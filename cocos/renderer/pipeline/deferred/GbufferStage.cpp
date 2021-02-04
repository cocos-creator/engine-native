/*
 Copyright (c) Huawei Technologies Co., Ltd. 2020-2021.

 https://www.cocos.com/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated engine source code (the "Software"), a limited,
 worldwide, royalty-free, non-assignable, revocable and non-exclusive license
 to use Cocos Creator solely to develop games on your target platforms. You shall
 not use Cocos Creator software for developing other software or tools that's
 used for developing games. You are not granted to publish, distribute,
 sublicense, and/or sell copies of Cocos Creator.

 The software or tools in this License Agreement are licensed, not sold.
 Xiamen Yaji Software Co., Ltd. reserves all rights not expressly granted to you.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */

#include "GbufferStage.h"
#include "GbufferFlow.h"
#include "../BatchedBuffer.h"
#include "../InstancedBuffer.h"
#include "../PlanarShadowQueue.h"
#include "../RenderBatchedQueue.h"
#include "../RenderInstancedQueue.h"
#include "../RenderQueue.h"
#include "../helper/SharedMemory.h"
#include "DeferredPipeline.h"
#include "gfx/GFXCommandBuffer.h"
#include "gfx/GFXDevice.h"
#include "gfx/GFXFramebuffer.h"
#include "gfx/GFXQueue.h"

namespace cc {
namespace pipeline {
namespace {
void SRGBToLinear(gfx::Color &out, const gfx::Color &gamma) {
    out.x = gamma.x * gamma.x;
    out.y = gamma.y * gamma.y;
    out.z = gamma.z * gamma.z;
}

void LinearToSRGB(gfx::Color &out, const gfx::Color &linear) {
    out.x = std::sqrt(linear.x);
    out.y = std::sqrt(linear.y);
    out.z = std::sqrt(linear.z);
}
} // namespace

RenderStageInfo GbufferStage::_initInfo = {
    "GbufferStage",
    static_cast<uint>(DeferredStagePriority::GBUFFER),
    static_cast<uint>(RenderFlowTag::SCENE),
    {{false, RenderQueueSortMode::FRONT_TO_BACK, {"default"}},
     {true, RenderQueueSortMode::BACK_TO_FRONT, {"default", "planarShadow"}}}};
const RenderStageInfo &GbufferStage::getInitializeInfo() { return GbufferStage::_initInfo; }

GbufferStage::GbufferStage() : RenderStage() {
    _batchedQueue = CC_NEW(RenderBatchedQueue);
    _instancedQueue = CC_NEW(RenderInstancedQueue);
}

GbufferStage::~GbufferStage() {
}

bool GbufferStage::initialize(const RenderStageInfo &info) {
    RenderStage::initialize(info);
    _renderQueueDescriptors = info.renderQueues;
    _phaseID = getPhaseID("deferred-gbuffer");
    return true;
}

void GbufferStage::activate(RenderPipeline *pipeline, RenderFlow *flow) {
    RenderStage::activate(pipeline, flow);
    for (const auto &descriptor : _renderQueueDescriptors) {
        uint phase = 0;
        for (const auto &stage : descriptor.stages) {
            phase |= getPhaseID(stage);
        }

        std::function<int(const RenderPass &, const RenderPass &)> sortFunc = opaqueCompareFn;
        switch (descriptor.sortMode) {
            case RenderQueueSortMode::BACK_TO_FRONT:
                sortFunc = transparentCompareFn;
                break;
            case RenderQueueSortMode::FRONT_TO_BACK:
                sortFunc = opaqueCompareFn;
            default:
                break;
        }

        RenderQueueCreateInfo info = {descriptor.isTransparent, phase, sortFunc};
        _renderQueues.emplace_back(CC_NEW(RenderQueue(std::move(info))));
    }

    _planarShadowQueue = CC_NEW(PlanarShadowQueue(_pipeline));
}

void GbufferStage::destroy() {
    CC_SAFE_DELETE(_batchedQueue);
    CC_SAFE_DELETE(_instancedQueue);
    CC_SAFE_DELETE(_planarShadowQueue);
    RenderStage::destroy();
}

void GbufferStage::render(Camera *camera) {
    _instancedQueue->clear();
    _batchedQueue->clear();
    auto pipeline = static_cast<DeferredPipeline *>(_pipeline);
    const auto &renderObjects = _pipeline->getPipelineSceneData()->getRenderObjects();
    if (renderObjects.empty()) {
        return;
    }

    for (auto queue : _renderQueues) {
        queue->clear();
    }

    uint m = 0, p = 0;
    size_t k = 0;
    for (size_t i = 0; i < renderObjects.size(); ++i) {
        const auto &ro = renderObjects[i];
        const auto model = ro.model;
        const auto subModelID = model->getSubModelID();
        const auto subModelCount = subModelID[0];
        for (m = 1; m <= subModelCount; ++m) {
            auto subModel = model->getSubModelView(subModelID[m]);
            for (p = 0; p < subModel->passCount; ++p) {
                const PassView *pass = subModel->getPassView(p);

                if (pass->phase != _phaseID) continue;
                if (pass->getBatchingScheme() == BatchingSchemes::INSTANCING) {
                    auto instancedBuffer = InstancedBuffer::get(subModel->passID[p]);
                    instancedBuffer->merge(model, subModel, p);
                    _instancedQueue->add(instancedBuffer);
                } else if (pass->getBatchingScheme() == BatchingSchemes::VB_MERGING) {
                    auto batchedBuffer = BatchedBuffer::get(subModel->passID[p]);
                    batchedBuffer->merge(subModel, p, model);
                    _batchedQueue->add(batchedBuffer);
                } else {
                    for (k = 0; k < _renderQueues.size(); k++) {
                        _renderQueues[k]->insertRenderPass(ro, m, p);
                    }
                }
            }
        }
    }
    for (auto queue : _renderQueues) {
        queue->sort();
    }

    auto cmdBuff = pipeline->getCommandBuffers()[0];

    _instancedQueue->uploadBuffers(cmdBuff);
    _batchedQueue->uploadBuffers(cmdBuff);

    // render area is not oriented
    _renderArea = pipeline->getRenderArea(camera, false);

    GbufferFlow *flow = dynamic_cast<GbufferFlow *>(getFlow());
    assert(flow != nullptr);
    auto framebuffer = flow->getFrameBuffer();
    auto renderPass = framebuffer->getRenderPass();

    cmdBuff->beginRenderPass(renderPass, framebuffer, _renderArea, _clearColors, camera->clearDepth, camera->clearStencil);
    cmdBuff->bindDescriptorSet(GLOBAL_SET, _pipeline->getDescriptorSet());

    _renderQueues[0]->recordCommandBuffer(_device, renderPass, cmdBuff);
    _instancedQueue->recordCommandBuffer(_device, renderPass, cmdBuff);
    _batchedQueue->recordCommandBuffer(_device, renderPass, cmdBuff);
    //_renderQueues[1]->recordCommandBuffer(_device, renderPass, cmdBuff);

    cmdBuff->endRenderPass();
}

} // namespace pipeline
} // namespace cc
