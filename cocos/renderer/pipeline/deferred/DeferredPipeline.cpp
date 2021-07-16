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

#include "DeferredPipeline.h"
#include "../SceneCulling.h"
#include "../shadow/ShadowFlow.h"
#include "MainFlow.h"
#include "gfx-base/GFXBuffer.h"
#include "gfx-base/GFXCommandBuffer.h"
#include "gfx-base/GFXDescriptorSet.h"
#include "gfx-base/GFXDevice.h"
#include "gfx-base/GFXFramebuffer.h"
#include "gfx-base/GFXQueue.h"
#include "gfx-base/GFXRenderPass.h"
#include "gfx-base/GFXSampler.h"
#include "gfx-base/GFXTexture.h"
#include "platform/Application.h"

namespace cc {
namespace pipeline {
namespace {
#define TO_VEC3(dst, src, offset)  \
    dst[offset]         = (src).x; \
    (dst)[(offset) + 1] = (src).y; \
    (dst)[(offset) + 2] = (src).z;
#define TO_VEC4(dst, src, offset)  \
    dst[offset]         = (src).x; \
    (dst)[(offset) + 1] = (src).y; \
    (dst)[(offset) + 2] = (src).z; \
    (dst)[(offset) + 3] = (src).w;
} // namespace

void srgbToLinear(gfx::Color *out, const gfx::Color &gamma) {
    out->x = gamma.x * gamma.x;
    out->y = gamma.y * gamma.y;
    out->z = gamma.z * gamma.z;
}

framegraph::StringHandle DeferredPipeline::_gbuffer[4] = {
    framegraph::FrameGraph::stringToHandle("gbufferAlbedo"),
    framegraph::FrameGraph::stringToHandle("gbufferPosition"),
    framegraph::FrameGraph::stringToHandle("gbufferNormal"),
    framegraph::FrameGraph::stringToHandle("gbufferEmissive")
};
framegraph::StringHandle DeferredPipeline::_depth = framegraph::FrameGraph::stringToHandle("depth");
framegraph::StringHandle DeferredPipeline::_lightingOut = framegraph::FrameGraph::stringToHandle("lightingOutput");
framegraph::StringHandle DeferredPipeline::_backBuffer = framegraph::FrameGraph::stringToHandle("backBuffer");

framegraph::StringHandle DeferredPipeline::_passGbuffer = framegraph::FrameGraph::stringToHandle("deferredGbufferPass");
framegraph::StringHandle DeferredPipeline::_passLighting = framegraph::FrameGraph::stringToHandle("deferredLightingPass");
framegraph::StringHandle DeferredPipeline::_passSspr = framegraph::FrameGraph::stringToHandle("deferredSSPRPass");
framegraph::StringHandle DeferredPipeline::_passPostprocess = framegraph::FrameGraph::stringToHandle("deferredPostPass");

bool DeferredPipeline::initialize(const RenderPipelineInfo &info) {
    RenderPipeline::initialize(info);

    if (_flows.empty()) {
        auto *shadowFlow = CC_NEW(ShadowFlow);
        shadowFlow->initialize(ShadowFlow::getInitializeInfo());
        _flows.emplace_back(shadowFlow);

        auto *mainFlow = CC_NEW(MainFlow);
        mainFlow->initialize(MainFlow::getInitializeInfo());
        _flows.emplace_back(mainFlow);
    }

    return true;
}

bool DeferredPipeline::activate() {
    _macros.setValue("CC_PIPELINE_TYPE", static_cast<float>(1.0));

    if (!RenderPipeline::activate()) {
        CC_LOG_ERROR("RenderPipeline active failed.");
        return false;
    }

    if (!activeRenderer()) {
        CC_LOG_ERROR("DeferredPipeline startup failed!");
        return false;
    }

    return true;
}

void DeferredPipeline::prepareFrameGraph() {
    // repare for the backbuffer
    _fg.getBlackboard().put(_backBuffer, _fg.importExternal(_backBuffer, _fgBackBuffer));
}

void DeferredPipeline::render(const vector<scene::Camera *> &cameras) {
    _commandBuffers[0]->begin();
    _pipelineUBO->updateGlobalUBO();
    _pipelineUBO->updateMultiCameraUBO(cameras);
    for (auto *camera : cameras) {
        _fg.reset();
        prepareFrameGraph();
        sceneCulling(this, camera);
        for (auto *const flow : _flows) {
            flow->render(camera);
        }

        _fg.compile();
        _fg.exportGraphViz("fg_vis.dot");
        _fg.execute();
        _pipelineUBO->incCameraUBOOffset();
    }

    _commandBuffers[0]->end();
    _device->flushCommands(_commandBuffers);
    _device->getQueue()->submit(_commandBuffers);
}

void DeferredPipeline::updateQuadVertexData(const gfx::Rect &renderArea) {
    if (_lastUsedRenderArea == renderArea) {
        return;
    }

    _lastUsedRenderArea = renderArea;
    float vbData[16]    = {0};
    genQuadVertexData(gfx::SurfaceTransform::IDENTITY, renderArea, vbData);
    _commandBuffers[0]->updateBuffer(_quadVBOffscreen, vbData);
}

void DeferredPipeline::genQuadVertexData(gfx::SurfaceTransform /*surfaceTransform*/, const gfx::Rect &renderArea, float *vbData) {
    float minX = float(renderArea.x) / _width;
    float maxX = float(renderArea.x + renderArea.width) / _width;
    float minY = float(renderArea.y) / _height;
    float maxY = float(renderArea.y + renderArea.height) / _height;
    if (_device->getCapabilities().screenSpaceSignY > 0) {
        std::swap(minY, maxY);
    }
    int n       = 0;
    vbData[n++] = -1.0;
    vbData[n++] = -1.0;
    vbData[n++] = minX; // uv
    vbData[n++] = maxY;
    vbData[n++] = 1.0;
    vbData[n++] = -1.0;
    vbData[n++] = maxX;
    vbData[n++] = maxY;
    vbData[n++] = -1.0;
    vbData[n++] = 1.0;
    vbData[n++] = minX;
    vbData[n++] = minY;
    vbData[n++] = 1.0;
    vbData[n++] = 1.0;
    vbData[n++] = maxX;
    vbData[n++] = minY;
}

bool DeferredPipeline::createQuadInputAssembler(gfx::Buffer **quadIB, gfx::Buffer **quadVB, gfx::InputAssembler **quadIA) {
    // step 1 create vertex buffer
    uint vbStride = sizeof(float) * 4;
    uint vbSize   = vbStride * 4;

    if (*quadVB == nullptr) {
        *quadVB = _device->createBuffer({gfx::BufferUsageBit::VERTEX | gfx::BufferUsageBit::TRANSFER_DST,
                                         gfx::MemoryUsageBit::HOST | gfx::MemoryUsageBit::DEVICE, vbSize, vbStride});
    }

    if (*quadVB == nullptr) {
        return false;
    }

    // step 2 create index buffer
    uint ibStride = 4;
    uint ibSize   = ibStride * 6;
    if (*quadIB == nullptr) {
        *quadIB = _device->createBuffer({gfx::BufferUsageBit::INDEX | gfx::BufferUsageBit::TRANSFER_DST,
                                         gfx::MemoryUsageBit::HOST | gfx::MemoryUsageBit::DEVICE, ibSize, ibStride});
    }

    if (*quadIB == nullptr) {
        return false;
    }

    unsigned int ibData[] = {0, 1, 2, 1, 3, 2};
    (*quadIB)->update(ibData, sizeof(ibData));

    // step 3 create input assembler
    gfx::InputAssemblerInfo info;
    info.attributes.push_back({"a_position", gfx::Format::RG32F});
    info.attributes.push_back({"a_texCoord", gfx::Format::RG32F});
    info.vertexBuffers.push_back(*quadVB);
    info.indexBuffer = *quadIB;
    *quadIA          = _device->createInputAssembler(info);
    return (*quadIA) != nullptr;
}

gfx::Rect DeferredPipeline::getRenderArea(scene::Camera *camera) {
    gfx::Rect renderArea;
    uint w = camera->window->hasOnScreenAttachments && static_cast<uint>(_device->getSurfaceTransform()) % 2 ? camera->height : camera->width;
    uint h = camera->window->hasOnScreenAttachments && static_cast<uint>(_device->getSurfaceTransform()) % 2 ? camera->width : camera->height;

    const auto &viewport = camera->viewPort;
    renderArea.x         = static_cast<int>(viewport.x * w);
    renderArea.y         = static_cast<int>(viewport.y * h);
    renderArea.width     = static_cast<uint>(viewport.z * w * _pipelineSceneData->getSharedData()->shadingScale);
    renderArea.height    = static_cast<uint>(viewport.w * h * _pipelineSceneData->getSharedData()->shadingScale);
    return renderArea;
}

void DeferredPipeline::destroyQuadInputAssembler() {
    if (_quadIB) {
        _quadIB->destroy();
        _quadIB = nullptr;
    }

    if (_quadVBOffscreen) {
        _quadVBOffscreen->destroy();
        _quadVBOffscreen = nullptr;
    }

    if (_quadIAOffscreen) {
        _quadIAOffscreen->destroy();
        _quadIAOffscreen = nullptr;
    }
}

bool DeferredPipeline::activeRenderer() {
    _commandBuffers.push_back(_device->getCommandBuffer());
    auto *const sharedData = _pipelineSceneData->getSharedData();

    gfx::SamplerInfo info{
        gfx::Filter::POINT,
        gfx::Filter::POINT,
        gfx::Filter::NONE,
        gfx::Address::CLAMP,
        gfx::Address::CLAMP,
        gfx::Address::CLAMP,
        {},
        {},
        {},
        {},
    };
    const uint  samplerHash = SamplerLib::genSamplerHash(info);
    gfx::Sampler *const sampler     = SamplerLib::getSampler(samplerHash);

    // Main light sampler binding
    this->_descriptorSet->bindSampler(SHADOWMAP::BINDING, sampler);
    this->_descriptorSet->bindTexture(SHADOWMAP::BINDING, getDefaultTexture());

    // Spot light sampler binding
    this->_descriptorSet->bindSampler(SPOTLIGHTINGMAP::BINDING, sampler);
    this->_descriptorSet->bindTexture(SPOTLIGHTINGMAP::BINDING, getDefaultTexture());

    _descriptorSet->update();

    // update global defines when all states initialized.
    _macros.setValue("CC_USE_HDR", static_cast<bool>(sharedData->isHDR));
    _macros.setValue("CC_SUPPORT_FLOAT_TEXTURE", _device->hasFeature(gfx::Feature::TEXTURE_FLOAT));

    if (!createQuadInputAssembler(&_quadIB, &_quadVBOffscreen, &_quadIAOffscreen)) {
        return false;
    }

    _width  = _device->getWidth();
    _height = _device->getHeight();

    return true;
}

void DeferredPipeline::resize(uint width, uint height) {
    if (_width == width && _height == height) {
        return;
    }
    _width  = width;
    _height = height;
    destroyDeferredData();
}

void DeferredPipeline::destroy() {
    destroyQuadInputAssembler();

    if (_descriptorSet) {
        _descriptorSet->getBuffer(UBOGlobal::BINDING)->destroy();
        _descriptorSet->getBuffer(UBOCamera::BINDING)->destroy();
        _descriptorSet->getBuffer(UBOShadow::BINDING)->destroy();
        _descriptorSet->getSampler(SHADOWMAP::BINDING)->destroy();
        _descriptorSet->getTexture(SHADOWMAP::BINDING)->destroy();
        _descriptorSet->getSampler(SPOTLIGHTINGMAP::BINDING)->destroy();
        _descriptorSet->getTexture(SPOTLIGHTINGMAP::BINDING)->destroy();
    }

    for (auto &it : _renderPasses) {
        it.second->destroy();
    }
    _renderPasses.clear();

    _commandBuffers.clear();

    _gbufferRenderPass  = nullptr;
    _lightingRenderPass = nullptr;

    RenderPipeline::destroy();
}

void DeferredPipeline::destroyDeferredData() {
}

gfx::Color DeferredPipeline::getClearcolor(scene::Camera *camera) {
    auto *const sceneData     = getPipelineSceneData();
    auto *const sharedData    = sceneData->getSharedData();
    gfx::Color clearColor = {0.0, 0.0, 0.0, 1.0};
    if (camera->clearFlag & static_cast<uint>(gfx::ClearFlagBit::COLOR)) {
        if (sharedData->isHDR) {
            srgbToLinear(&clearColor, camera->clearColor);
            const auto scale = sharedData->fpScale / camera->exposure;
            clearColor.x *= scale;
            clearColor.y *= scale;
            clearColor.z *= scale;
        } else {
            clearColor = camera->clearColor;
        }
    }

    clearColor.w = 0;
    return clearColor;
}

} // namespace pipeline
} // namespace cc
