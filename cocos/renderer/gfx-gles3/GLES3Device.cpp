/****************************************************************************
     Copyright (c) 2019-2021 Xiamen Yaji Software Co., Ltd.

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

#include "GLES3Std.h"

#include "GLES3Buffer.h"
#include "GLES3CommandBuffer.h"
#include "GLES3Context.h"
#include "GLES3DescriptorSet.h"
#include "GLES3DescriptorSetLayout.h"
#include "GLES3Device.h"
#include "GLES3Framebuffer.h"
#include "GLES3GlobalBarrier.h"
#include "GLES3InputAssembler.h"
#include "GLES3PipelineLayout.h"
#include "GLES3PipelineState.h"
#include "GLES3PrimaryCommandBuffer.h"
#include "GLES3Queue.h"
#include "GLES3RenderPass.h"
#include "GLES3Sampler.h"
#include "GLES3Shader.h"
#include "GLES3Swapchain.h"
#include "GLES3Texture.h"
#include "base/Utils.h"
#include "gfx-gles-common/GLESCommandPool.h"

// when capturing GLES commands (RENDERDOC_HOOK_EGL=1, default value)
// RenderDoc doesn't support this extension during replay
#define ALLOW_MULTISAMPLED_RENDER_TO_TEXTURE_ON_DESKTOP 0

namespace cc {
namespace gfx {

GLES3Device *GLES3Device::instance = nullptr;

GLES3Device *GLES3Device::getInstance() {
    return GLES3Device::instance;
}

GLES3Device::GLES3Device() {
    _api        = API::GLES3;
    _deviceName = "GLES3";

    GLES3Device::instance = this;
}

GLES3Device::~GLES3Device() {
    GLES3Device::instance = nullptr;
}

bool GLES3Device::doInit(const DeviceInfo & /*info*/) {
    if (!gles3wInit()) {
        return false;
    }

    EGLDisplay eglDisplay = nullptr;
    EGL_CHECK(eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY));

    if (eglInitialize(eglDisplay, &_majorVer, &_minorVer) != EGL_TRUE) {
        CC_LOG_ERROR("Couldn't initialize EGLDisplay.");
        return false;
    }

    EGL_CHECK(eglBindAPI(EGL_OPENGL_ES_API));

    EGLint pbufferAttribs[5];
    pbufferAttribs[0] = EGL_WIDTH;
    pbufferAttribs[1] = 64;
    pbufferAttribs[2] = EGL_HEIGHT;
    pbufferAttribs[3] = 64;
    pbufferAttribs[4] = EGL_NONE;

    const EGLint confAttr[] =
        {
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT_KHR,
            EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
            EGL_RED_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_BLUE_SIZE, 8,
            EGL_ALPHA_SIZE, 8,
            EGL_DEPTH_SIZE, 8,
            EGL_STENCIL_SIZE, 8,
            EGL_NONE};
    EGLint    numConfigs = 0;
    EGLConfig eglConf    = nullptr;
    if (!eglChooseConfig(eglDisplay, confAttr, &eglConf, 1, &numConfigs)) {
        CC_LOG_ERROR("some config is wrong");
    } else {
        CC_LOG_INFO("all configs is OK");
    }

    int       iConfigs   = 0;
    EGLConfig eglConfig  = nullptr;
    auto *    eglSurface = eglCreatePbufferSurface(eglDisplay, eglConf, pbufferAttribs);

    const EGLint ctxAttr[] = {
        EGL_CONTEXT_CLIENT_VERSION, 3,
        EGL_NONE};
    auto *context = eglCreateContext(eglDisplay, eglConfig, nullptr, ctxAttr);

    eglMakeCurrent(eglDisplay, nullptr, nullptr, context);
    initByPBuffer();
    eglDestroyContext(eglDisplay, context);
    eglDestroySurface(eglDisplay, eglSurface);

    QueueInfo queueInfo;
    queueInfo.type = QueueType::GRAPHICS;
    _queue         = createQueue(queueInfo);

    CommandBufferInfo cmdBuffInfo;
    cmdBuffInfo.type  = CommandBufferType::PRIMARY;
    cmdBuffInfo.queue = _queue;
    _cmdBuff          = createCommandBuffer(cmdBuffInfo);

    return true;
}

void GLES3Device::doDestroy() {
    CC_SAFE_DELETE(_gpuFramebufferCacheMap)
    CC_SAFE_DELETE(_gpuConstantRegistry)
    CC_SAFE_DELETE(_gpuStagingBufferPool)
    CC_SAFE_DELETE(_gpuStateCache)

    CCASSERT(!_memoryStatus.bufferSize, "Buffer memory leaked");
    CCASSERT(!_memoryStatus.textureSize, "Texture memory leaked");

    CC_SAFE_DESTROY(_cmdBuff)
    CC_SAFE_DESTROY(_queue)
}

void GLES3Device::acquire(Swapchain *const *swapchains, uint32_t count) {
    if (count == 1) {
        if (_currentSwapchain != swapchains[0]) {
            _currentSwapchain = static_cast<GLES3Swapchain *>(swapchains[0]);
            auto *context     = _currentSwapchain->getContext();
            context->makeCurrent();
            _gpuStateCache->reset();
        }
    } else if (count > 1) {
        CC_LOG_ERROR("OpenGL ES does not support obtaining multiple swapchains at the same time");
    }
}

void GLES3Device::present() {
    if (_currentSwapchain) {
        auto *queue   = static_cast<GLES3Queue *>(_queue);
        _numDrawCalls = queue->_numDrawCalls;
        _numInstances = queue->_numInstances;
        _numTriangles = queue->_numTriangles;

        _currentSwapchain->getContext()->present();

        // Clear queue stats
        queue->_numDrawCalls = 0;
        queue->_numInstances = 0;
        queue->_numTriangles = 0;

        stagingBufferPool()->reset();
    }
}

CommandBuffer *GLES3Device::createCommandBuffer(const CommandBufferInfo &info, bool hasAgent) {
    if (hasAgent || info.type == CommandBufferType::PRIMARY) return CC_NEW(GLES3PrimaryCommandBuffer);
    return CC_NEW(GLES3CommandBuffer);
}

Queue *GLES3Device::createQueue() {
    return CC_NEW(GLES3Queue);
}

Buffer *GLES3Device::createBuffer() {
    return CC_NEW(GLES3Buffer);
}

Texture *GLES3Device::createTexture() {
    return CC_NEW(GLES3Texture);
}

Sampler *GLES3Device::createSampler() {
    return CC_NEW(GLES3Sampler);
}

Shader *GLES3Device::createShader() {
    return CC_NEW(GLES3Shader);
}

InputAssembler *GLES3Device::createInputAssembler() {
    return CC_NEW(GLES3InputAssembler);
}

RenderPass *GLES3Device::createRenderPass() {
    return CC_NEW(GLES3RenderPass);
}

Framebuffer *GLES3Device::createFramebuffer() {
    return CC_NEW(GLES3Framebuffer);
}

DescriptorSet *GLES3Device::createDescriptorSet() {
    return CC_NEW(GLES3DescriptorSet);
}

DescriptorSetLayout *GLES3Device::createDescriptorSetLayout() {
    return CC_NEW(GLES3DescriptorSetLayout);
}

PipelineLayout *GLES3Device::createPipelineLayout() {
    return CC_NEW(GLES3PipelineLayout);
}

PipelineState *GLES3Device::createPipelineState() {
    return CC_NEW(GLES3PipelineState);
}

GlobalBarrier *GLES3Device::createGlobalBarrier() {
    return CC_NEW(GLES3GlobalBarrier);
}

TextureBarrier *GLES3Device::createTextureBarrier() {
    return CC_NEW(TextureBarrier);
}

void GLES3Device::copyBuffersToTexture(const uint8_t *const *buffers, Texture *dst, const BufferTextureCopy *regions, uint count) {
    cmdFuncGLES3CopyBuffersToTexture(this, buffers, static_cast<GLES3Texture *>(dst)->gpuTexture(), regions, count);
}

void GLES3Device::copyTextureToBuffers(Texture *srcTexture, uint8_t *const *buffers, const BufferTextureCopy *regions, uint count) {
    cmdFuncGLES3CopyTextureToBuffers(this, srcTexture ? static_cast<GLES3Texture *>(srcTexture)->gpuTexture() : nullptr, buffers, regions, count);
}

Swapchain *GLES3Device::createSwapchain() {
    return CC_NEW(GLES3Swapchain);
}

void GLES3Device::onSwapchainCreated(GLES3Swapchain *swapchain) {
    _swapchains.insert(swapchain);

    if (!_currentSwapchain) {
        auto *context = static_cast<GLES3Swapchain *>(swapchain)->getContext();
        initByContext(context);
        _currentSwapchain = swapchain;
    }
}

void GLES3Device::onSwapchainDestroyed(GLES3Swapchain *swapchain) {
    _swapchains.erase(swapchain);
}

void GLES3Device::initByPBuffer() {
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, reinterpret_cast<GLint *>(&_caps.maxVertexAttributes));
    glGetIntegerv(GL_MAX_VERTEX_UNIFORM_VECTORS, reinterpret_cast<GLint *>(&_caps.maxVertexUniformVectors));
    glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_VECTORS, reinterpret_cast<GLint *>(&_caps.maxFragmentUniformVectors));
    glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, reinterpret_cast<GLint *>(&_caps.maxUniformBufferBindings));
    glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, reinterpret_cast<GLint *>(&_caps.maxUniformBlockSize));
    glGetIntegerv(GL_MAX_DRAW_BUFFERS, reinterpret_cast<GLint *>(&_caps.maxColorRenderTargets));
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, reinterpret_cast<GLint *>(&_caps.maxTextureUnits));
    glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, reinterpret_cast<GLint *>(&_caps.maxVertexTextureUnits));
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, reinterpret_cast<GLint *>(&_caps.maxTextureSize));
    glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE, reinterpret_cast<GLint *>(&_caps.maxCubeMapTextureSize));
    glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, reinterpret_cast<GLint *>(&_caps.uboOffsetAlignment));
    if (_minorVer) {
        glGetIntegerv(GL_MAX_IMAGE_UNITS, reinterpret_cast<GLint *>(&_caps.maxImageUnits));
        glGetIntegerv(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, reinterpret_cast<GLint *>(&_caps.maxShaderStorageBlockSize));
        glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, reinterpret_cast<GLint *>(&_caps.maxShaderStorageBufferBindings));
        glGetIntegerv(GL_MAX_COMPUTE_SHARED_MEMORY_SIZE, reinterpret_cast<GLint *>(&_caps.maxComputeSharedMemorySize));
        glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, reinterpret_cast<GLint *>(&_caps.maxComputeWorkGroupInvocations));
        glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, reinterpret_cast<GLint *>(&_caps.maxComputeWorkGroupSize.x));
        glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, reinterpret_cast<GLint *>(&_caps.maxComputeWorkGroupSize.y));
        glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, reinterpret_cast<GLint *>(&_caps.maxComputeWorkGroupSize.z));
        glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, reinterpret_cast<GLint *>(&_caps.maxComputeWorkGroupCount.x));
        glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, reinterpret_cast<GLint *>(&_caps.maxComputeWorkGroupCount.y));
        glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, reinterpret_cast<GLint *>(&_caps.maxComputeWorkGroupCount.z));
    }
    String extStr = reinterpret_cast<const char *>(glGetString(GL_EXTENSIONS));
    _extensions   = StringUtil::split(extStr, " ");

    _features[toNumber(Feature::TEXTURE_FLOAT)]           = true;
    _features[toNumber(Feature::TEXTURE_HALF_FLOAT)]      = true;
    _features[toNumber(Feature::FORMAT_R11G11B10F)]       = true;
    _features[toNumber(Feature::INSTANCED_ARRAYS)]        = true;
    _features[toNumber(Feature::MULTIPLE_RENDER_TARGETS)] = true;
    _features[toNumber(Feature::BLEND_MINMAX)]            = true;
    _features[toNumber(Feature::ELEMENT_INDEX_UINT)]      = true;

    _multithreadedSubmission = false;

    if (_minorVer) {
        _features[toNumber(Feature::COMPUTE_SHADER)] = true;
    }

    if (checkExtension("EXT_sRGB")) {
        _features[toNumber(Feature::FORMAT_SRGB)] = true;
    }

    if (checkExtension("color_buffer_float")) {
        _features[toNumber(Feature::COLOR_FLOAT)] = true;
    }

    if (checkExtension("color_buffer_half_float")) {
        _features[toNumber(Feature::COLOR_HALF_FLOAT)] = true;
    }

    if (checkExtension("texture_float_linear")) {
        _features[toNumber(Feature::TEXTURE_FLOAT_LINEAR)] = true;
    }

    if (checkExtension("texture_half_float_linear")) {
        _features[toNumber(Feature::TEXTURE_HALF_FLOAT_LINEAR)] = true;
    }

    String compressedFmts;

    if (checkExtension("compressed_ETC1")) {
        _features[toNumber(Feature::FORMAT_ETC1)] = true;
        compressedFmts += "etc1 ";
    }

    _features[toNumber(Feature::FORMAT_ETC2)] = true;
    compressedFmts += "etc2 ";

    if (checkExtension("texture_compression_pvrtc")) {
        _features[toNumber(Feature::FORMAT_PVRTC)] = true;
        compressedFmts += "pvrtc ";
    }

    if (checkExtension("texture_compression_astc")) {
        _features[toNumber(Feature::FORMAT_ASTC)] = true;
        compressedFmts += "astc ";
    }
    _features[toNumber(Feature::FORMAT_RGB8)] = true;
    CC_LOG_INFO("COMPRESSED_FORMATS: %s", compressedFmts.c_str());
}

void GLES3Device::initByContext(GLES3Context *context) {
    _gpuStateCache          = CC_NEW(GLES3GPUStateCache);
    _gpuStagingBufferPool   = CC_NEW(GLES3GPUStagingBufferPool);
    _gpuConstantRegistry    = CC_NEW(GLES3GPUConstantRegistry);
    _gpuFramebufferCacheMap = CC_NEW(GLES3GPUFramebufferCacheMap(_gpuStateCache));

    context->makeCurrent(true);

#if CC_PLATFORM != CC_PLATFORM_WINDOWS && CC_PLATFORM != CC_PLATFORM_MAC_OSX || ALLOW_MULTISAMPLED_RENDER_TO_TEXTURE_ON_DESKTOP
    if (checkExtension("multisampled_render_to_texture")) {
        if (checkExtension("multisampled_render_to_texture2")) {
            _gpuConstantRegistry->mMSRT = MSRTSupportLevel::LEVEL2;
        } else {
            _gpuConstantRegistry->mMSRT = MSRTSupportLevel::LEVEL1;
        }
    }
#endif

    String fbfLevelStr = "NONE";
    if (checkExtension("framebuffer_fetch")) {
        String nonCoherent = "framebuffer_fetch_non";

        auto it = std::find_if(_extensions.begin(), _extensions.end(), [&nonCoherent](auto &ext) {
            return ext.find(nonCoherent) != String::npos;
        });

        if (it != _extensions.end()) {
            if (*it == CC_TOSTR(GL_EXT_shader_framebuffer_fetch_non_coherent)) {
                _gpuConstantRegistry->mFBF = FBFSupportLevel::NON_COHERENT_EXT;
                fbfLevelStr                = "NON_COHERENT_EXT";
            } else if (*it == CC_TOSTR(GL_QCOM_shader_framebuffer_fetch_noncoherent)) {
                _gpuConstantRegistry->mFBF = FBFSupportLevel::NON_COHERENT_QCOM;
                fbfLevelStr                = "NON_COHERENT_QCOM";
                GL_CHECK(glEnable(GL_FRAMEBUFFER_FETCH_NONCOHERENT_QCOM));
            }
        } else if (checkExtension(CC_TOSTR(GL_EXT_shader_framebuffer_fetch))) {
            // we only care about EXT_shader_framebuffer_fetch, the ARM version does not support MRT
            _gpuConstantRegistry->mFBF = FBFSupportLevel::COHERENT;
            fbfLevelStr                = "COHERENT";
        }
    }

// PVRVFrame has issues on their PLS support
#if CC_PLATFORM != CC_PLATFORM_WINDOWS && CC_PLATFORM != CC_PLATFORM_MAC_OSX
    if (checkExtension("pixel_local_storage")) {
        if (checkExtension("pixel_local_storage2")) {
            _gpuConstantRegistry->mPLS = PLSSupportLevel::LEVEL2;
        } else {
            _gpuConstantRegistry->mPLS = PLSSupportLevel::LEVEL1;
        }
        glGetIntegerv(GL_MAX_SHADER_PIXEL_LOCAL_STORAGE_SIZE_EXT, reinterpret_cast<GLint *>(&_gpuConstantRegistry->mPLSsize));
    }
#endif

    _gpuConstantRegistry->glMinorVersion     = context->minorVer();
    _gpuConstantRegistry->defaultFramebuffer = context->getDefaultFramebuffer();

    _renderer = reinterpret_cast<const char *>(glGetString(GL_RENDERER));
    _vendor   = reinterpret_cast<const char *>(glGetString(GL_VENDOR));
    _version  = reinterpret_cast<const char *>(glGetString(GL_VERSION));

    CC_LOG_INFO("GLES3 device initialized.");
    CC_LOG_INFO("RENDERER: %s", _renderer.c_str());
    CC_LOG_INFO("VENDOR: %s", _vendor.c_str());
    CC_LOG_INFO("VERSION: %s", _version.c_str());
    //CC_LOG_INFO("SCREEN_SIZE: %d x %d", _width, _height);

    CC_LOG_INFO("PIXEL_LOCAL_STORAGE: level %d, size %d", _gpuConstantRegistry->mPLS, _gpuConstantRegistry->mPLSsize);
    CC_LOG_INFO("FRAMEBUFFER_FETCH: %s", fbfLevelStr.c_str());

    _gpuStateCache->initialize(_caps.maxTextureUnits, _caps.maxImageUnits, _caps.maxUniformBufferBindings, _caps.maxShaderStorageBufferBindings, _caps.maxVertexAttributes);
}

GLES3Context *GLES3Device::getContextForSharing() const {
    if (!_swapchains.empty()) {
        auto *swapchain = static_cast<GLES3Swapchain *>(*_swapchains.begin());
        return swapchain->getContext();
    }
    return nullptr;
}

} // namespace gfx
} // namespace cc
