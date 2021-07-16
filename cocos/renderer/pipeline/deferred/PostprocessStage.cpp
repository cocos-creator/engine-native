/****************************************************************************
 Copyright (c) 2020-2021 Huawei Technologies Co., Ltd.

 http://www.cocos.com

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
****************************************************************************/

#include "PostprocessStage.h"
#include "../PipelineStateManager.h"
#include "../RenderQueue.h"
#include "../forward/UIPhase.h"
#include "DeferredPipeline.h"
#include "gfx-base/GFXCommandBuffer.h"
#include "gfx-base/GFXDevice.h"
#include "gfx-base/GFXFramebuffer.h"
#include "scene/SubModel.h"
#include "frame-graph/PassNodeBuilder.h"
#include "frame-graph/DevicePass.h"
#include "frame-graph/Resource.h"

namespace cc {
namespace pipeline {
RenderStageInfo PostprocessStage::initInfo = {
    "PostprocessStage",
    static_cast<uint>(DeferredStagePriority::POSTPROCESS),
    0,
    {{true, RenderQueueSortMode::BACK_TO_FRONT, {"default"}}},
};
const RenderStageInfo &PostprocessStage::getInitializeInfo() { return PostprocessStage::initInfo; }


PostprocessStage::PostprocessStage() {
    _uiPhase = CC_NEW(UIPhase);
}

bool PostprocessStage::initialize(const RenderStageInfo &info) {
    RenderStage::initialize(info);
    _renderQueueDescriptors = info.renderQueues;
    return true;
}

void PostprocessStage::activate(RenderPipeline *pipeline, RenderFlow *flow) {
    RenderStage::activate(pipeline, flow);
    _uiPhase->activate(pipeline);
    _phaseID = getPhaseID("default");

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
}

void PostprocessStage::destroy() {
}

void PostprocessStage::render(scene::Camera *camera) {
    struct renderData {
        framegraph::TextureHandle lightingOut;      // read from lighting output
        framegraph::TextureHandle depth;            // read from depth
        framegraph::TextureHandle backBuffer;       // write to back buffer
    };

    auto postSetup = [&] (framegraph::PassNodeBuilder &builder, renderData &data) {
        data.lightingOut = builder.read(framegraph::TextureHandle(builder.readFromBlackboard(DeferredPipeline::_lightingOut)));
        builder.writeToBlackboard(DeferredPipeline::_lightingOut, data.lightingOut);

        // depth is as an attachment
        framegraph::RenderTargetAttachment::Descriptor depthAttachInfo;
        depthAttachInfo.usage       = framegraph::RenderTargetAttachment::Usage::DEPTH_STENCIL;
        depthAttachInfo.loadOp      = gfx::LoadOp::LOAD;
        depthAttachInfo.clearColor  = gfx::Color();
        depthAttachInfo.endAccesses = {gfx::AccessType::DEPTH_STENCIL_ATTACHMENT_WRITE};

        data.depth = builder.write(framegraph::TextureHandle(builder.readFromBlackboard(DeferredPipeline::_depth)), depthAttachInfo);
        builder.writeToBlackboard(DeferredPipeline::_depth, data.depth);

        // backbuffer is as an attachment
        framegraph::RenderTargetAttachment::Descriptor colorAttachmentInfo;
        colorAttachmentInfo.usage       = framegraph::RenderTargetAttachment::Usage::COLOR;
        colorAttachmentInfo.loadOp      = gfx::LoadOp::DISCARD;
        colorAttachmentInfo.clearColor  = _clearColors[0];
        colorAttachmentInfo.endAccesses = {gfx::AccessType::PRESENT};

        data.backBuffer = builder.write(framegraph::TextureHandle(builder.readFromBlackboard(DeferredPipeline::_backBuffer)), colorAttachmentInfo);
        builder.writeToBlackboard(DeferredPipeline::_backBuffer, data.backBuffer);
    };

    auto postExec = [&] (renderData const &data, const framegraph::DevicePassResourceTable &table) {
        DeferredPipeline *pipeline = static_cast<DeferredPipeline *>(RenderPipeline::getInstance());
        assert(pipeline != nullptr);
        gfx::RenderPass *renderPass = table.getDevicePass()->getRenderPass().get();
        assert(renderPass != nullptr);

        // bind descriptor
        gfx::Texture *lightingOut    = (gfx::Texture *)(table.getRead(data.lightingOut));

        gfx::SamplerInfo info{
            gfx::Filter::LINEAR,
            gfx::Filter::LINEAR,
            gfx::Filter::NONE,
            gfx::Address::CLAMP,
            gfx::Address::CLAMP,
            gfx::Address::CLAMP,
        };
        const auto  samplerHash = SamplerLib::genSamplerHash(info);
        auto *const sampler     = SamplerLib::getSampler(samplerHash);
        pipeline->getDescriptorSet()->bindSampler(static_cast<uint>(PipelineGlobalBindings::SAMPLER_LIGHTING_RESULTMAP), sampler);
        pipeline->getDescriptorSet()->bindTexture(static_cast<uint>(PipelineGlobalBindings::SAMPLER_LIGHTING_RESULTMAP), lightingOut);

        auto *cmdBf = pipeline->getCommandBuffers()[0];
        uint const globalOffsets[] = {pipeline->getPipelineUBO()->getCurrentCameraUBOOffset()};
        cmdBf->bindDescriptorSet(static_cast<uint>(SetIndex::GLOBAL), pipeline->getDescriptorSet(), static_cast<uint>(std::size(globalOffsets)), globalOffsets);

        // post proces
        auto *const  sceneData     = pipeline->getPipelineSceneData();
        scene::Pass *pv            = sceneData->getSharedData()->deferredPostPass;
        gfx::Shader *sd            = sceneData->getSharedData()->deferredPostPassShader;

        gfx::InputAssembler *ia = pipeline->getQuadIAOffScreen();
        gfx::PipelineState * pso = PipelineStateManager::getOrCreatePipelineState(pv, sd, ia, renderPass);
        assert(pso != nullptr);

        cmdBf->bindPipelineState(pso);
        cmdBf->bindInputAssembler(ia);
        cmdBf->draw(ia);
    };

    auto *      pipeline      = static_cast<DeferredPipeline *>(_pipeline);
    pipeline->getFrameGraph().addPass<renderData>(IP_POSTPROCESS, DeferredPipeline::_passPostprocess, postSetup, postExec);
    pipeline->getFrameGraph().presentFromBlackboard(DeferredPipeline::_backBuffer);
}
} // namespace pipeline
} // namespace cc
