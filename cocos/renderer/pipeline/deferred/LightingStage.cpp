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

#include "LightingStage.h"
#include "../BatchedBuffer.h"
#include "../InstancedBuffer.h"
#include "../PipelineStateManager.h"
#include "../PlanarShadowQueue.h"
#include "../RenderBatchedQueue.h"
#include "../RenderInstancedQueue.h"
#include "../RenderQueue.h"
#include "DeferredPipeline.h"
#include "gfx-base/GFXCommandBuffer.h"
#include "gfx-base/GFXDescriptorSet.h"
#include "gfx-base/GFXDevice.h"
#include "gfx-base/GFXFramebuffer.h"
#include "gfx-base/GFXQueue.h"
#include "scene/RenderScene.h"
#include "scene/Sphere.h"
#include "scene/SphereLight.h"
#include "../Define.h"

namespace cc {
namespace pipeline {
namespace {
void srgbToLinear(gfx::Color *out, const gfx::Color &gamma) {
    out->x = gamma.x * gamma.x;
    out->y = gamma.y * gamma.y;
    out->z = gamma.z * gamma.z;
}

void linearToSrgb(gfx::Color *out, const gfx::Color &linear) {
    out->x = std::sqrt(linear.x);
    out->y = std::sqrt(linear.y);
    out->z = std::sqrt(linear.z);
}
} // namespace

static const String StageName = "LightingStage";

static const uint _maxReflectorSize =  5;
framegraph::StringHandle _reflectTexHandle = framegraph::FrameGraph::stringToHandle("reflectionTex");
framegraph::StringHandle _denoiseTexHandle[_maxReflectorSize];
framegraph::StringHandle _ssprClearPass[_maxReflectorSize];
framegraph::StringHandle _ssprCompReflectPass[_maxReflectorSize];
framegraph::StringHandle _ssprCompDenoisePass[_maxReflectorSize];
framegraph::StringHandle _ssprRenderPass[_maxReflectorSize];

static void initStrHandle () {
    std::string tmp;
    for (int i = 0; i < _maxReflectorSize; ++i) {
        tmp = std::string("denoiseTexureHandle") + std::to_string(i);
        _denoiseTexHandle[i] = framegraph::FrameGraph::stringToHandle(tmp.c_str());

        tmp = std::string("ssprClearPss") + std::to_string(i);
        _ssprClearPass[i] = framegraph::FrameGraph::stringToHandle(tmp.c_str());

        tmp = std::string("ssprReflectPass") + std::to_string(i);
        _ssprCompReflectPass[i] = framegraph::FrameGraph::stringToHandle(tmp.c_str());

        tmp = std::string("ssprDenoisePass") + std::to_string(i);
        _ssprCompDenoisePass[i] = framegraph::FrameGraph::stringToHandle(tmp.c_str());

        tmp = std::string("ssprRenderPass") + std::to_string(i);
        _ssprRenderPass[i] = framegraph::FrameGraph::stringToHandle(tmp.c_str());
    }
}

RenderStageInfo LightingStage::initInfo = {
    StageName,
    static_cast<uint>(DeferredStagePriority::LIGHTING),
    static_cast<uint>(RenderFlowTag::SCENE),
};

const RenderStageInfo &LightingStage::getInitializeInfo() { return LightingStage::initInfo; }

LightingStage::LightingStage() = default;

LightingStage::~LightingStage() {
    _deferredLitsBufs->destroy();
    _deferredLitsBufs = nullptr;
    _deferredLitsBufView->destroy();
    _deferredLitsBufView = nullptr;
}

bool LightingStage::initialize(const RenderStageInfo &info) {
    RenderStage::initialize(info);
    _renderQueueDescriptors = info.renderQueues;
    _phaseID                = getPhaseID("default");
    _defPhaseID             = getPhaseID("deferred");
    _reflectionPhaseID      = getPhaseID("reflection");
    initStrHandle();
    return true;
}

void LightingStage::gatherLights(scene::Camera *camera) {
    auto *pipeline = dynamic_cast<DeferredPipeline *>(_pipeline);
    if (!pipeline) {
        return;
    }

    auto *const sceneData  = _pipeline->getPipelineSceneData();
    auto *const sharedData = sceneData->getSharedData();

    gfx::CommandBuffer *cmdBuf = pipeline->getCommandBuffers()[0];
    const auto *        scene  = camera->scene;

    scene::Sphere sphere;
    auto          exposure   = camera->exposure;
    uint          idx        = 0;
    int           elementLen = sizeof(cc::Vec4) / sizeof(float);
    uint          fieldLen   = elementLen * _maxDeferredLights;
    uint          offset     = 0;
    cc::Vec4      tmpArray;

    uint i = 0;
    for (auto *light : scene->getSphereLights()) {
        if (i >= _maxDeferredLights) {
            break;
        }

        const auto &position = light->getPosition();
        sphere.setCenter(position);
        sphere.setRadius(light->getRange());
        if (!sphere.sphereFrustum(camera->frustum)) {
            continue;
        }
        // position
        offset                       = idx * elementLen;
        _lightBufferData[offset]     = position.x;
        _lightBufferData[offset + 1] = position.y;
        _lightBufferData[offset + 2] = position.z;
        _lightBufferData[offset + 3] = 0;

        // color
        const auto &color = light->getColor();
        offset            = idx * elementLen + fieldLen;
        tmpArray.set(color.x, color.y, color.z, 0);
        if (light->getUseColorTemperature()) {
            const auto &colorTemperatureRGB = light->getColorTemperatureRGB();
            tmpArray.x *= colorTemperatureRGB.x;
            tmpArray.y *= colorTemperatureRGB.y;
            tmpArray.z *= colorTemperatureRGB.z;
        }

        if (sharedData->isHDR) {
            tmpArray.w = light->getIlluminance() * sharedData->fpScale * _lightMeterScale;
        } else {
            tmpArray.w = light->getIlluminance() * exposure * _lightMeterScale;
        }

        _lightBufferData[offset + 0] = tmpArray.x;
        _lightBufferData[offset + 1] = tmpArray.y;
        _lightBufferData[offset + 2] = tmpArray.z;
        _lightBufferData[offset + 3] = tmpArray.w;

        // size range angle
        offset                       = idx * elementLen + fieldLen * 2;
        _lightBufferData[offset]     = light->getSize();
        _lightBufferData[offset + 1] = light->getRange();
        _lightBufferData[offset + 2] = 0;

        ++i;
        ++idx;
    }

    i = 0;
    for (auto *light : scene->getSpotLights()) {
        if (i >= _maxDeferredLights) {
            break;
        }

        const auto &position = light->getPosition();
        sphere.setCenter(position);
        sphere.setRadius(light->getRange());
        if (!sphere.sphereFrustum(camera->frustum)) {
            continue;
        }
        // position
        offset                       = idx * elementLen;
        _lightBufferData[offset]     = position.x;
        _lightBufferData[offset + 1] = position.y;
        _lightBufferData[offset + 2] = position.z;
        _lightBufferData[offset + 3] = 1;

        // color
        offset            = idx * elementLen + fieldLen;
        const auto &color = light->getColor();
        tmpArray.set(color.x, color.y, color.z, 0);
        if (light->getUseColorTemperature()) {
            const auto &colorTemperatureRGB = light->getColorTemperatureRGB();
            tmpArray.x *= colorTemperatureRGB.x;
            tmpArray.y *= colorTemperatureRGB.y;
            tmpArray.z *= colorTemperatureRGB.z;
        }

        if (sharedData->isHDR) {
            tmpArray.w = light->getIlluminance() * sharedData->fpScale * _lightMeterScale;
        } else {
            tmpArray.w = light->getIlluminance() * exposure * _lightMeterScale;
        }

        _lightBufferData[offset + 0] = tmpArray.x;
        _lightBufferData[offset + 1] = tmpArray.y;
        _lightBufferData[offset + 2] = tmpArray.z;
        _lightBufferData[offset + 3] = tmpArray.w;

        // size range angle
        offset                       = idx * elementLen + fieldLen * 2;
        _lightBufferData[offset]     = light->getSize();
        _lightBufferData[offset + 1] = light->getRange();
        _lightBufferData[offset + 2] = light->getSpotAngle();

        // dir
        const auto &direction        = light->getDirection();
        offset                       = idx * elementLen + fieldLen * 3;
        _lightBufferData[offset]     = direction.x;
        _lightBufferData[offset + 1] = direction.y;
        _lightBufferData[offset + 2] = direction.z;

        ++i;
        ++idx;
    }

    // the count of lights is set to cc_lightDir[0].w
    _lightBufferData[fieldLen * 3 + 3] = static_cast<float>(idx);
    cmdBuf->updateBuffer(_deferredLitsBufs, _lightBufferData.data());
}

void LightingStage::initLightingBuffer() {
    auto *const device = _pipeline->getDevice();

    // color/pos/dir/angle 都是vec4存储, 最后一个vec4只要x存储光源个数
    uint totalSize = sizeof(Vec4) * 4 * _maxDeferredLights;
    totalSize      = static_cast<uint>(std::ceil(static_cast<float>(totalSize) / device->getCapabilities().uboOffsetAlignment) * device->getCapabilities().uboOffsetAlignment);

    // create lighting buffer and view
    if (_deferredLitsBufs == nullptr) {
        gfx::BufferInfo bfInfo = {
            gfx::BufferUsageBit::UNIFORM | gfx::BufferUsageBit::TRANSFER_DST,
            gfx::MemoryUsageBit::HOST | gfx::MemoryUsageBit::DEVICE,
            totalSize,
            static_cast<uint>(device->getCapabilities().uboOffsetAlignment),
        };
        _deferredLitsBufs = device->createBuffer(bfInfo);
        assert(_deferredLitsBufs != nullptr);
    }

    if (_deferredLitsBufView == nullptr) {
        gfx::BufferViewInfo bvInfo = {_deferredLitsBufs, 0, totalSize};
        _deferredLitsBufView       = device->createBuffer(bvInfo);
        assert(_deferredLitsBufView != nullptr);
        _descriptorSet->bindBuffer(static_cast<uint>(ModelLocalBindings::UBO_FORWARD_LIGHTS), _deferredLitsBufView);
    }

    _lightBufferData.resize(totalSize / sizeof(float));
}

void LightingStage::activate(RenderPipeline *pipeline, RenderFlow *flow) {
    RenderStage::activate(pipeline, flow);

    auto *const device = pipeline->getDevice();

    for (const auto &descriptor : _renderQueueDescriptors) {
        uint                  phase    = convertPhase(descriptor.stages);
        RenderQueueSortFunc   sortFunc = convertQueueSortFunc(descriptor.sortMode);
        RenderQueueCreateInfo info     = {descriptor.isTransparent, phase, sortFunc};
        _renderQueues.emplace_back(CC_NEW(RenderQueue(std::move(info))));
    }

    // create descriptorset/layout
    gfx::DescriptorSetLayoutInfo layoutInfo = {localDescriptorSetLayout.bindings};
    _descLayout                             = device->createDescriptorSetLayout(layoutInfo);

    gfx::DescriptorSetInfo setInfo = {_descLayout};
    _descriptorSet                 = device->createDescriptorSet(setInfo);

    // create lighting buffer and view
    initLightingBuffer();

    _planarShadowQueue = CC_NEW(PlanarShadowQueue(_pipeline));

    // create reflection resource
    RenderQueueCreateInfo info = {true, _reflectionPhaseID, transparentCompareFn};
    _reflectionComp            = new ReflectionComp();

    gfx::ColorAttachment cAttch = {
        gfx::Format::RGBA8,
        gfx::SampleCount::X1,
        gfx::LoadOp::LOAD,
        gfx::StoreOp::STORE,
        {gfx::AccessType::FRAGMENT_SHADER_READ_TEXTURE},
        {gfx::AccessType::FRAGMENT_SHADER_READ_TEXTURE},
    };

    gfx::RenderPassInfo reflectionPassInfo;
    reflectionPassInfo.colorAttachments.push_back(cAttch);

    reflectionPassInfo.depthStencilAttachment = {
        _device->getDepthStencilFormat(),
        gfx::SampleCount::X1,
        gfx::LoadOp::LOAD,
        gfx::StoreOp::DISCARD,
        gfx::LoadOp::DISCARD,
        gfx::StoreOp::DISCARD,
        {gfx::AccessType::DEPTH_STENCIL_ATTACHMENT_WRITE},
    };

    _reflectionPass        = _device->createRenderPass(reflectionPassInfo);
    _reflectionRenderQueue = CC_NEW(RenderQueue(std::move(info)));
}

void LightingStage::destroy() {
    CC_SAFE_DELETE(_planarShadowQueue);
    CC_SAFE_DELETE(_reflectionRenderQueue);
    RenderStage::destroy();

    if (_reflectionPass != nullptr) {
        _reflectionPass->destroy();
        CC_SAFE_DELETE(_reflectionPass);
    }

    CC_SAFE_DELETE(_reflectionComp);
}

void LightingStage::recordCommands(DeferredPipeline *pipeline, gfx::RenderPass *renderPass) {
    auto *const sceneData     = pipeline->getPipelineSceneData();

    auto *cmdBuff = pipeline->getCommandBuffers()[0];
    vector<uint> dynamicOffsets = {0};
    cmdBuff->bindDescriptorSet(static_cast<uint>(SetIndex::LOCAL), _descriptorSet, dynamicOffsets);

    // gbuffer descriptorset setup
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

    pipeline->getDescriptorSet()->bindSampler(static_cast<uint>(PipelineGlobalBindings::SAMPLER_GBUFFER_ALBEDOMAP), sampler);
    pipeline->getDescriptorSet()->bindSampler(static_cast<uint>(PipelineGlobalBindings::SAMPLER_GBUFFER_POSITIONMAP), sampler);
    pipeline->getDescriptorSet()->bindSampler(static_cast<uint>(PipelineGlobalBindings::SAMPLER_GBUFFER_NORMALMAP), sampler);
    pipeline->getDescriptorSet()->bindSampler(static_cast<uint>(PipelineGlobalBindings::SAMPLER_GBUFFER_EMISSIVEMAP), sampler);
    pipeline->getDescriptorSet()->update();

    uint const globalOffsets[] = {_pipeline->getPipelineUBO()->getCurrentCameraUBOOffset()};
    cmdBuff->bindDescriptorSet(static_cast<uint>(SetIndex::GLOBAL), pipeline->getDescriptorSet(), static_cast<uint>(std::size(globalOffsets)), globalOffsets);
    // get pso and draw quad
    scene::Pass *pass   = sceneData->getSharedData()->deferredLightPass;
    gfx::Shader *shader = sceneData->getSharedData()->deferredLightPassShader;
    gfx::InputAssembler *inputAssembler = pipeline->getQuadIAOffScreen();
    gfx::PipelineState *pState         = PipelineStateManager::getOrCreatePipelineState(
        pass, shader, inputAssembler, renderPass);
    assert(pState != nullptr);

    cmdBuff->bindPipelineState(pState);
    cmdBuff->bindInputAssembler(inputAssembler);
    cmdBuff->draw(inputAssembler);

    // transparent
    putTransparentObj2Queue();
    for (auto *queue : _renderQueues) {
        queue->sort();
        queue->recordCommandBuffer(_device, renderPass, cmdBuff);
    }

    _planarShadowQueue->recordCommandBuffer(_device, renderPass, cmdBuff);
}

void LightingStage::fgLightingPass(scene::Camera *camera) {
    // lighting info, ubo
    gatherLights(camera);
    _descriptorSet->update();

    struct renderData {
        framegraph::TextureHandle gbuffer[4];   // read from gbuffer stage
        framegraph::TextureHandle depth;        // read from gbuffer stage
        framegraph::TextureHandle lightOutput;  // output texture
    };

    auto *      pipeline      = static_cast<DeferredPipeline *>(_pipeline);
    auto lightingSetup = [&] (framegraph::PassNodeBuilder &builder, renderData &data) {
        // read gbuffer
        for (int i = 0; i < 4; i++) {
            data.gbuffer[i] = builder.read(framegraph::TextureHandle(builder.readFromBlackboard(DeferredPipeline::_gbuffer[i])));
            builder.writeToBlackboard(DeferredPipeline::_gbuffer[i], data.gbuffer[i]);
        }

        // read depth, as an attachment
        framegraph::RenderTargetAttachment::Descriptor depthAttachmentInfo;
        depthAttachmentInfo.usage       = framegraph::RenderTargetAttachment::Usage::DEPTH;
        depthAttachmentInfo.loadOp      = gfx::LoadOp::LOAD;
        depthAttachmentInfo.clearColor  = gfx::Color();
        depthAttachmentInfo.endAccesses = {gfx::AccessType::DEPTH_STENCIL_ATTACHMENT_READ};

        //data.depth = builder.read(framegraph::TextureHandle(builder.readFromBlackboard(DeferredPipeline::_depth)));
        //data.depth = builder.read(framegraph::TextureHandle(builder.readFromBlackboard(DeferredPipeline::_depth)), depthAttachmentInfo);
        //builder.writeToBlackboard(DeferredPipeline::_depth, data.depth);

        // write to lighting output
        framegraph::RenderTargetAttachment::Descriptor colorAttachmentInfo;
        colorAttachmentInfo.usage       = framegraph::RenderTargetAttachment::Usage::COLOR;
        colorAttachmentInfo.loadOp      = gfx::LoadOp::DISCARD;
        colorAttachmentInfo.clearColor  = gfx::Color();
        //colorAttachmentInfo.endAccesses = {gfx::AccessType::COLOR_ATTACHMENT_READ};
        colorAttachmentInfo.endAccesses = {gfx::AccessType::COLOR_ATTACHMENT_READ, gfx::AccessType::COMPUTE_SHADER_READ_TEXTURE};

        framegraph::Texture::Descriptor colorTexInfo;
        colorTexInfo.format = gfx::Format::RGBA16F;
        colorTexInfo.usage  = gfx::TextureUsageBit::COLOR_ATTACHMENT | gfx::TextureUsageBit::SAMPLED;
        colorTexInfo.width  = _device->getWidth();
        colorTexInfo.height = _device->getHeight();

        builder.create(data.lightOutput, DeferredPipeline::_lightingOut, colorTexInfo);
        data.lightOutput = builder.write(data.lightOutput, colorAttachmentInfo);
        builder.writeToBlackboard(DeferredPipeline::_lightingOut, data.lightOutput);

        // set render area
        auto renderArea = pipeline->getRenderArea(camera);
        gfx::Viewport viewport{ renderArea.x, renderArea.y, renderArea.width, renderArea.height, 0.F, 1.F};
        builder.setViewport(viewport, renderArea);
    };

    auto lightingExec = [] (renderData const &data, const framegraph::DevicePassResourceTable &table) {
        DeferredPipeline *pipeline = static_cast<DeferredPipeline *>(RenderPipeline::getInstance());
        assert(pipeline != nullptr);
        LightingStage *stage = static_cast<LightingStage *>(pipeline->getRenderstageByName(StageName));
        assert(stage != nullptr);
        gfx::RenderPass *renderPass = table.getDevicePass()->getRenderPass().get();
        assert(renderPass != nullptr);

        // bind descriptors
        gfx::Texture *gbufferAlbedo    = (gfx::Texture *)(table.getRead(data.gbuffer[0]));
        gfx::Texture *gbufferPosition  = (gfx::Texture *)(table.getRead(data.gbuffer[1]));
        gfx::Texture *gbufferNormal    = (gfx::Texture *)(table.getRead(data.gbuffer[2]));
        gfx::Texture *gbufferEmissive   = (gfx::Texture *)(table.getRead(data.gbuffer[3]));
        pipeline->getDescriptorSet()->bindTexture(static_cast<uint>(PipelineGlobalBindings::SAMPLER_GBUFFER_ALBEDOMAP), gbufferAlbedo);
        pipeline->getDescriptorSet()->bindTexture(static_cast<uint>(PipelineGlobalBindings::SAMPLER_GBUFFER_POSITIONMAP), gbufferPosition);
        pipeline->getDescriptorSet()->bindTexture(static_cast<uint>(PipelineGlobalBindings::SAMPLER_GBUFFER_NORMALMAP), gbufferNormal);
        pipeline->getDescriptorSet()->bindTexture(static_cast<uint>(PipelineGlobalBindings::SAMPLER_GBUFFER_EMISSIVEMAP), gbufferEmissive);
        pipeline->getDescriptorSet()->update();

        stage->recordCommands(pipeline, renderPass);
    };

    pipeline->getFrameGraph().addPass<renderData>(IP_LIGHTING, DeferredPipeline::_passLighting, lightingSetup, lightingExec);
}

void LightingStage::putTransparentObj2Queue() {
    for (auto *queue : _renderQueues) {
        queue->clear();
    }

    auto *const sceneData     = _pipeline->getPipelineSceneData();
    const auto &renderObjects = sceneData->getRenderObjects();

    uint   m = 0;
    uint   p = 0;
    size_t k = 0;
    for (auto ro : renderObjects) {
        const auto *const model = ro.model;
        for (auto *subModel : model->getSubModels()) {
            for (auto *pass : subModel->getPasses()) {
                // TODO(xwx): need fallback of unlit and gizmo material.
                if (pass->getPhase() != _phaseID && pass->getPhase() != _defPhaseID) continue;
                for (k = 0; k < _renderQueues.size(); k++) {
                    _renderQueues[k]->insertRenderPass(ro, m, p);
                }
            }
        }
    }
}

void LightingStage::fgSsprPass(scene::Camera *camera) {
    // The max reflector objects is 5.
    // for each reflector, there are 4 pass, clear pass/reflection pass/denoise pass/render pass
    // each reflector has its own denoise texture, and will be used in render pass
    // All the reflectors use the same reflect texture which is temp resoruce for the reflectors, and the reflect texture should be cleared for each reflector
    // we first calculate denoise textures of all reflectors one by one, the first 3 pass will be executed
    // then we render all reflectors one by one, the last render pass will be executed

    if (!_device->hasFeature(gfx::Feature::COMPUTE_SHADER)) {
        return;
    }

    _denoiseIndex = 0;
    matViewProj = camera->matViewProj;
    _reflectionElems.clear();

    // step 1 prepare clear model's reflection texture pass. if imageclear command is supported, this pass can be replaced by it
    uint minSize = 512;
    uint width = _device->getWidth();
    uint height = _device->getHeight();
    if (height < width) {
        width = minSize * width / height;
        height = minSize;
    } else {
        width = minSize;
        height = minSize * height / width;
    }

    struct DataClear {
        framegraph::TextureHandle reflection;
    };

    auto clearSetup = [&] (framegraph::PassNodeBuilder &builder, DataClear &data) {
        framegraph::RenderTargetAttachment::Descriptor colorAttachmentInfo;
        colorAttachmentInfo.usage       = framegraph::RenderTargetAttachment::Usage::COLOR;
        colorAttachmentInfo.loadOp      = gfx::LoadOp::CLEAR;
        colorAttachmentInfo.clearColor  = gfx::Color();
        colorAttachmentInfo.endAccesses = {gfx::AccessType::COLOR_ATTACHMENT_READ};

        framegraph::Texture::Descriptor colorTexInfo;
        colorTexInfo.format = gfx::Format::RGBA8;
        colorTexInfo.usage  = gfx::TextureUsageBit::STORAGE | gfx::TextureUsageBit::SAMPLED | gfx::TextureUsageBit::TRANSFER_SRC | gfx::TextureUsageBit::TRANSFER_DST;
        colorTexInfo.width  = width;
        colorTexInfo.height = height;

        builder.create(data.reflection, _reflectTexHandle, colorTexInfo);
        data.reflection = builder.write(data.reflection, colorAttachmentInfo);
        builder.writeToBlackboard(_reflectTexHandle, data.reflection);
    };

    auto clearExec = [&] (DataClear const &data, const framegraph::DevicePassResourceTable &table) {};

    // step 2 prepare compute the reflection pass, contain 1 dispatch commands, compute pipeline
    struct DataCompReflect {
        framegraph::TextureHandle reflection;       // compute result texture
        framegraph::TextureHandle lightingOut;      // read from lighting pass output texture
        framegraph::TextureHandle gbufferPosition;  // read from gbuffer.positon texture
    };

    auto compReflectSetup = [&] (framegraph::PassNodeBuilder &builder, DataCompReflect &data) {
        // if there is no attachment in the pass, renderpass willn't be created
        // read lightingout as input
        data.lightingOut = builder.read(framegraph::TextureHandle(builder.readFromBlackboard(DeferredPipeline::_lightingOut)));
        builder.writeToBlackboard(DeferredPipeline::_lightingOut, data.lightingOut);

        // read gbufferPosition as input
        data.gbufferPosition = builder.read(framegraph::TextureHandle(builder.readFromBlackboard(DeferredPipeline::_gbuffer[1])));
        builder.writeToBlackboard(DeferredPipeline::_gbuffer[1], data.gbufferPosition);

        // write to reflection
        data.reflection = framegraph::TextureHandle(builder.readFromBlackboard(_reflectTexHandle));
        if (!data.reflection.isValid()) {
            framegraph::Texture::Descriptor colorTexInfo;
            colorTexInfo.format = gfx::Format::RGBA8;
            colorTexInfo.usage  = gfx::TextureUsageBit::STORAGE | gfx::TextureUsageBit::SAMPLED | gfx::TextureUsageBit::TRANSFER_SRC | gfx::TextureUsageBit::TRANSFER_DST;
            colorTexInfo.width  = width;
            colorTexInfo.height = height;
            builder.create(data.reflection, _reflectTexHandle, colorTexInfo);
        }

        data.reflection = builder.write(data.reflection);
        builder.writeToBlackboard(_reflectTexHandle, data.reflection);
    };

    auto compReflectExec = [&] (DataCompReflect const &data, const framegraph::DevicePassResourceTable &table) {
        DeferredPipeline *pipeline = static_cast<DeferredPipeline *>(RenderPipeline::getInstance());
        assert(pipeline != nullptr);
        LightingStage *stage = static_cast<LightingStage *>(pipeline->getRenderstageByName(StageName));
        assert(stage != nullptr);

        ReflectionComp *reflectionComp = stage->getReflectionComp();
        if (!reflectionComp->isInitialized()) {
            reflectionComp->init(pipeline->getDevice(), nullptr, nullptr, nullptr,
            stage->matViewProj, 8, 8);
        }
        gfx::Texture *texReflection  = (gfx::Texture *)(table.getWrite(data.reflection));
        gfx::Texture *texLightingOut = (gfx::Texture *)(table.getRead(data.lightingOut));
        gfx::Texture *texPositon     = (gfx::Texture *)(table.getRead(data.gbufferPosition));

        // step 1 pipelinebarrier before exec
        auto *cmdBuff = pipeline->getCommandBuffers()[0];
        cmdBuff->pipelineBarrier(reflectionComp->getBarrierPre());

        // step 2 bind descriptors
        // layout(set = 0, binding = 0) uniform Constants {  mat4 matViewProj; vec2 texSize; };
        // layout(set = 0, binding = 1) uniform sampler2D lightingTex;
        // layout(set = 0, binding = 2) uniform sampler2D worldPositionTex;
        // layout(set = 0, binding = 3, rgba8) writeonly uniform lowp image2D reflectionTex;
        // set 0
        gfx::DescriptorSet *reflectDesc = reflectionComp->getDescriptorSet();
        gfx::Sampler *sampler = reflectionComp->getSampler();
        gfx::Buffer *constBuffer = reflectionComp->getConstantsBuffer();

        reflectDesc->bindBuffer(0, constBuffer);
        reflectDesc->bindSampler(1, sampler);
        reflectDesc->bindTexture(1, texLightingOut);
        reflectDesc->bindSampler(2, sampler);
        reflectDesc->bindTexture(2, texPositon);
        reflectDesc->bindTexture(3, texReflection);
        reflectDesc->update();

        // set 1, subModel->getDescriptorSet(0)
        cmdBuff->bindPipelineState(const_cast<gfx::PipelineState *>(reflectionComp->getPipelineState()));
        cmdBuff->bindDescriptorSet(0, reflectDesc);
        cmdBuff->bindDescriptorSet(1, stage->getRendElement().set);
        cmdBuff->dispatch(reflectionComp->getDispatchInfo());
    };

    // step 3 prepare compute the denoise pass, contain 1 dispatch commands, compute pipeline
    struct DataCompDenoise {
        framegraph::TextureHandle denoise;          // each reflector has its own denoise texture
        framegraph::TextureHandle reflection;       // the texture from last pass
    };

    auto compDenoiseSetup = [&] (framegraph::PassNodeBuilder &builder, DataCompDenoise &data) {
        // if there is no attachment in the pass, renderpass willn't be created
        // read _reflectTexHandle
        data.reflection = builder.read(framegraph::TextureHandle(builder.readFromBlackboard(_reflectTexHandle)));
        builder.writeToBlackboard(_reflectTexHandle, data.reflection);

        // write to reflection
        framegraph::Texture::Descriptor colorTexInfo;
        colorTexInfo.format = gfx::Format::RGBA8;
        colorTexInfo.usage  = gfx::TextureUsageBit::STORAGE | gfx::TextureUsageBit::SAMPLED | gfx::TextureUsageBit::TRANSFER_SRC;
        colorTexInfo.width  = width;
        colorTexInfo.height = height;
        builder.create(data.denoise, _denoiseTexHandle[_denoiseIndex], colorTexInfo);

        data.denoise = builder.write(data.denoise);
        builder.writeToBlackboard(_denoiseTexHandle[_denoiseIndex], data.denoise);
    };

    auto compDenoiseExec = [&] (DataCompDenoise const &data, const framegraph::DevicePassResourceTable &table) {
        DeferredPipeline *pipeline = static_cast<DeferredPipeline *>(RenderPipeline::getInstance());
        assert(pipeline != nullptr);
        LightingStage *stage = static_cast<LightingStage *>(pipeline->getRenderstageByName(StageName));
        assert(stage != nullptr);

        gfx::Texture *denoiseTex  = (gfx::Texture *)(table.getWrite(data.denoise));
        gfx::Texture *reflectionTex  = (gfx::Texture *)(table.getRead(data.reflection));

        ReflectionComp *reflectionComp = stage->getReflectionComp();

        // pipeline barrier
        auto *cmdBuff = pipeline->getCommandBuffers()[0];
        cmdBuff->pipelineBarrier(nullptr, const_cast<gfx::TextureBarrierList &>(reflectionComp->getBarrierBeforeDenoise()), {reflectionTex, denoiseTex});

        // bind descriptorset
        reflectionComp->getDenoiseDescriptorSet()->bindTexture(0, reflectionTex);
        reflectionComp->getDenoiseDescriptorSet()->bindSampler(0, reflectionComp->getSampler());
        reflectionComp->getDenoiseDescriptorSet()->update();

        gfx::SamplerInfo samplerInfo;
        samplerInfo.minFilter = gfx::Filter::LINEAR;
        samplerInfo.magFilter = gfx::Filter::LINEAR;
        samplerInfo.addressU = gfx::Address::CLAMP;
        samplerInfo.addressV = gfx::Address::CLAMP;
        samplerInfo.addressW = gfx::Address::CLAMP;
        gfx::Sampler *sample = pipeline->getDevice()->createSampler(samplerInfo);

        stage->getRendElement().set->bindTexture(static_cast<uint>(ModelLocalBindings::STORAGE_REFLECTION), denoiseTex);
        stage->getRendElement().set->bindSampler(static_cast<uint>(ModelLocalBindings::STORAGE_REFLECTION), sample);
        stage->getRendElement().set->update();

        cmdBuff->bindPipelineState(const_cast<gfx::PipelineState *>(reflectionComp->getDenoisePipelineState()));
        cmdBuff->bindDescriptorSet(0, const_cast<gfx::DescriptorSet *>(reflectionComp->getDenoiseDescriptorSet()));
        cmdBuff->bindDescriptorSet(1, stage->getRendElement().set);
        cmdBuff->dispatch(reflectionComp->getDenioseDispatchInfo());

        // pipeline barrier
        // dispatch -> fragment
        cmdBuff->pipelineBarrier(nullptr, reflectionComp->getBarrierAfterDenoise(), {denoiseTex});

        gfx::GlobalBarrierInfo infoPre = {
            {
                gfx::AccessType::DEPTH_STENCIL_ATTACHMENT_WRITE, gfx::AccessType::COLOR_ATTACHMENT_WRITE,
            },
            {
                gfx::AccessType::DEPTH_STENCIL_ATTACHMENT_READ,gfx::AccessType::DEPTH_STENCIL_ATTACHMENT_WRITE, gfx::AccessType::COLOR_ATTACHMENT_WRITE, gfx::AccessType::COLOR_ATTACHMENT_READ,
            }};

        gfx::GlobalBarrier *_barrierPre = pipeline->getDevice()->createGlobalBarrier(infoPre);
        cmdBuff->pipelineBarrier(_barrierPre);

        stage->addDenoiseIndex();
    };

    // step 4 prepare render reflector objects pass, graphics pipeline
    struct DataRender {
        framegraph::TextureHandle denoise;      // input texture
        framegraph::TextureHandle lightingOut;  // attachment, load and write
        framegraph::TextureHandle depth;        // attachment, load and write
    };

    auto renderSetup = [&] (framegraph::PassNodeBuilder &builder, DataRender &data) {
        data.denoise = builder.read(framegraph::TextureHandle(builder.readFromBlackboard(_denoiseTexHandle[_denoiseIndex])));
        builder.writeToBlackboard(_denoiseTexHandle[_denoiseIndex], data.denoise);

        // write lightingOut, as an attachment
        framegraph::RenderTargetAttachment::Descriptor colorAttachmentInfo;
        colorAttachmentInfo.usage       = framegraph::RenderTargetAttachment::Usage::COLOR;
        colorAttachmentInfo.loadOp      = gfx::LoadOp::LOAD;
        colorAttachmentInfo.clearColor  = gfx::Color();
        colorAttachmentInfo.endAccesses = {gfx::AccessType::FRAGMENT_SHADER_READ_TEXTURE};

        data.lightingOut = builder.write(framegraph::TextureHandle(builder.readFromBlackboard(DeferredPipeline::_lightingOut)), colorAttachmentInfo);
        builder.writeToBlackboard(DeferredPipeline::_lightingOut, data.lightingOut);

        // read depth, as an attachment
        framegraph::RenderTargetAttachment::Descriptor depthAttachmentInfo;
        depthAttachmentInfo.usage       = framegraph::RenderTargetAttachment::Usage::DEPTH;
        depthAttachmentInfo.loadOp      = gfx::LoadOp::LOAD;
        depthAttachmentInfo.clearColor  = gfx::Color();
        depthAttachmentInfo.endAccesses = {gfx::AccessType::FRAGMENT_SHADER_READ_DEPTH_STENCIL_INPUT_ATTACHMENT};

        data.depth = builder.write(framegraph::TextureHandle(builder.readFromBlackboard(DeferredPipeline::_depth)), depthAttachmentInfo);
        builder.writeToBlackboard(DeferredPipeline::_depth, data.depth);
    };

    auto renderExec = [&] (DataRender const &data, const framegraph::DevicePassResourceTable &table) {
        DeferredPipeline *pipeline = static_cast<DeferredPipeline *>(RenderPipeline::getInstance());
        assert(pipeline != nullptr);
        LightingStage *stage = static_cast<LightingStage *>(pipeline->getRenderstageByName(StageName));
        assert(stage != nullptr);
        auto *cmdBuff = pipeline->getCommandBuffers()[0];
        RenderElem elem = stage->getRendElement();

        // bind descriptor
        cmdBuff->bindDescriptorSet(static_cast<uint>(SetIndex::GLOBAL), pipeline->getDescriptorSet());

        gfx::DescriptorSet *descLocal = elem.set;
        gfx::Texture *denoiseTex  = (gfx::Texture *)(table.getRead(data.denoise));

        gfx::SamplerInfo samplerInfo ;
        samplerInfo.minFilter = gfx::Filter::LINEAR;
        samplerInfo.magFilter = gfx::Filter::LINEAR;
        samplerInfo.addressU = gfx::Address::CLAMP;
        samplerInfo.addressV = gfx::Address::CLAMP;
        samplerInfo.addressW = gfx::Address::CLAMP;
        gfx::Sampler *sample = pipeline->getDevice()->createSampler(samplerInfo);

        descLocal->bindTexture(static_cast<uint>(ModelLocalBindings::SAMPLER_REFLECTION), denoiseTex);
        descLocal->bindSampler(static_cast<uint>(ModelLocalBindings::SAMPLER_REFLECTION), sample);
        descLocal->update();
        cmdBuff->bindDescriptorSet(static_cast<uint>(SetIndex::LOCAL), descLocal);

        stage->getReflectRenderQueue()->clear();
        stage->getReflectRenderQueue()->insertRenderPass(elem.renderObject, elem.modelIndex, elem.passIndex);

        gfx::RenderPass *renderPass = table.getDevicePass()->getRenderPass().get();
        stage->getReflectRenderQueue()->sort();
        stage->getReflectRenderQueue()->recordCommandBuffer(pipeline->getDevice(), renderPass, cmdBuff);
    };

    // step 5 add framegraph passes
    auto *const sceneData     = _pipeline->getPipelineSceneData();
    const auto &renderObjects = sceneData->getRenderObjects();
    uint   m = 0;
    uint   p = 0;
    for (const auto &ro : renderObjects) {
        const auto *model = ro.model;
        const auto& subModels = model->getSubModels();
        for (m = 0; m < subModels.size(); ++m) {
            auto& subModel = subModels[m];
            const auto& passes = subModel->getPasses();
            auto passCount = passes.size();
            for (p = 0; p < passCount; ++p) {
                auto* pass = passes[p];
                if (pass->getPhase() == _reflectionPhaseID) {
                    RenderElem elem = {ro, subModel->getDescriptorSet(), m, p};
                    _reflectionElems.push_back(elem);
                }
            }
        }
    }

    uint insertPoint = IP_SSPR;
    DeferredPipeline *pipeline = static_cast<DeferredPipeline *>(RenderPipeline::getInstance());
    for (int i = 0; i < _reflectionElems.size(); ++i) {
        // add clear and comp passes here
        //pipeline->getFrameGraph().addPass<DataClear>(insertPoint++, _ssprClearPass[i], clearSetup, clearExec);
        pipeline->getFrameGraph().addPass<DataCompReflect>(insertPoint++, _ssprCompReflectPass[i], compReflectSetup, compReflectExec);
        pipeline->getFrameGraph().addPass<DataCompDenoise>(insertPoint++, _ssprCompDenoisePass[i], compDenoiseSetup, compDenoiseExec);
    }

    for (int i = 0; i < _reflectionElems.size(); ++i) {
        // add graphic pass here
        pipeline->getFrameGraph().addPass<DataRender>(insertPoint++, _ssprRenderPass[i], renderSetup, renderExec);
    }
}

void LightingStage::render(scene::Camera *camera) {
    fgLightingPass(camera);
    fgSsprPass(camera);
}

void LightingStage::renderBAK(scene::Camera *camera) {
    #if 0
    auto *      pipeline      = static_cast<DeferredPipeline *>(_pipeline);
    auto *const sceneData     = _pipeline->getPipelineSceneData();
    auto *const sharedData    = sceneData->getSharedData();
    const auto &renderObjects = sceneData->getRenderObjects();

    if (renderObjects.empty()) {
        return;
    }

    auto *cmdBuff = pipeline->getCommandBuffers()[0];

    if (_device->hasFeature(gfx::Feature::COMPUTE_SHADER)) {
        uint   m = 0;
        uint   p = 0;
        size_t k = 0;
        for (const auto &ro : renderObjects) {
            const auto *model = ro.model;
            for (auto *subModel : model->getSubModels()) {
                for (auto *pass : subModel->getPasses()) {
                    if (pass->getPhase() != _reflectionPhaseID) continue;
                    // dispatch for reflection
                    gfx::Texture *denoiseTex = subModel->getDescriptorSet()->getTexture(uint(ModelLocalBindings::STORAGE_REFLECTION));
                    if (!_reflectionComp->isInitialized()) {
                        _reflectionComp->init(_pipeline->getDevice(),
                                              pipeline->getDeferredRenderData()->lightingRenderTarget,
                                              pipeline->getDeferredRenderData()->gbufferFrameBuffer->getColorTextures()[1],
                                              denoiseTex,
                                              camera->matViewProj, 8, 8);
                    }
                    gfx::Rect clearRenderArea = {0, 0, _reflectionComp->getReflectionTex()->getWidth(), _reflectionComp->getReflectionTex()->getHeight()};
                    cmdBuff->beginRenderPass(const_cast<gfx::RenderPass *>(_reflectionComp->getClearPass()), const_cast<gfx::Framebuffer *>(_reflectionComp->getClearFramebuffer()), clearRenderArea, &clearColor, 0, 0);
                    cmdBuff->endRenderPass();

                    cmdBuff->pipelineBarrier(_reflectionComp->getBarrierPre());
                    cmdBuff->bindPipelineState(const_cast<gfx::PipelineState *>(_reflectionComp->getPipelineState()));
                    cmdBuff->bindDescriptorSet(0, const_cast<gfx::DescriptorSet *>(_reflectionComp->getDescriptorSet()));
                    cmdBuff->bindDescriptorSet(1, subModel->getDescriptorSet());

                    cmdBuff->dispatch(_reflectionComp->getDispatchInfo());

                    cmdBuff->pipelineBarrier(nullptr, const_cast<gfx::TextureBarrierList &>(_reflectionComp->getBarrierBeforeDenoise()), {const_cast<gfx::Texture *>(_reflectionComp->getReflectionTex()), denoiseTex});

                    cmdBuff->bindPipelineState(const_cast<gfx::PipelineState *>(_reflectionComp->getDenoisePipelineState()));
                    cmdBuff->bindDescriptorSet(0, const_cast<gfx::DescriptorSet *>(_reflectionComp->getDenoiseDescriptorSet()));
                    cmdBuff->bindDescriptorSet(1, subModel->getDescriptorSet());
                    cmdBuff->dispatch(_reflectionComp->getDenioseDispatchInfo());
                    cmdBuff->pipelineBarrier(nullptr, _reflectionComp->getBarrierAfterDenoise(), {denoiseTex});
                }
            }
        }
    }

    cmdBuff->beginRenderPass(_reflectionPass, frameBuffer, renderArea, &clearColor,
                             camera->clearDepth, camera->clearStencil);

    cmdBuff->bindDescriptorSet(static_cast<uint>(SetIndex::GLOBAL), pipeline->getDescriptorSet());

    // reflection
    _reflectionRenderQueue->clear();

    uint m = 0;
    uint p = 0;
    for (const auto &ro : renderObjects) {
        const auto *model = ro.model;
        for (auto *subModel : model->getSubModels()) {
            for (auto *pass : subModel->getPasses()) {
                if (pass->getPhase() != _reflectionPhaseID) continue;
                _reflectionRenderQueue->insertRenderPass(ro, m, p);
            }
        }
    }

    _reflectionRenderQueue->sort();
    _reflectionRenderQueue->recordCommandBuffer(_device, renderPass, cmdBuff);

    cmdBuff->endRenderPass();
    #endif
}

RenderElem LightingStage::getRendElement() {
    assert(_denoiseIndex != _reflectionElems.size());
    return _reflectionElems[_denoiseIndex];
}

} // namespace pipeline
} // namespace cc
