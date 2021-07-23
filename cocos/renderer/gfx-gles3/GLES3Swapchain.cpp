#include "GLES3Swapchain.h"
#include "GLES3Texture.h"
#include "GLES3Device.h"

namespace cc {
namespace gfx {

void GLES3Swapchain::doInit(const SwapchainInfo& info) {
    GLES3ContextInfo contextInfo;
    contextInfo.vsyncMode = info.vsyncMode;
    contextInfo.windowHandle = info.windowHandle;
    contextInfo.sharedCtx = GLES3Device::getInstance()->getContextForSharing();
    contextInfo.msaaEnabled  = info.samples != SampleCount::ONE;
    _context = CC_NEW(GLES3Context);
    _context->init(contextInfo);

    _colorTexture = CC_NEW(GLES3Texture);
    SwapchainTextureInfo colorTextureInfo;
    colorTextureInfo.swapchain = this;
    colorTextureInfo.width     = info.width;
    colorTextureInfo.height    = info.height;
    colorTextureInfo.samples   = info.samples;
    colorTextureInfo.format    = _context->getColorFormat();
    Swapchain::initTexture(colorTextureInfo, _colorTexture);

    _depthStencilTexture = CC_NEW(GLES3Texture);
    SwapchainTextureInfo depthStencilTextureInfo;
    depthStencilTextureInfo.swapchain = this;
    depthStencilTextureInfo.width     = info.width;
    depthStencilTextureInfo.height    = info.height;
    depthStencilTextureInfo.samples   = info.samples;
    depthStencilTextureInfo.format    = _context->getDepthStencilFormat();
    Swapchain::initTexture(depthStencilTextureInfo, _depthStencilTexture);

    GLES3Device::getInstance()->onSwapchainCreated(this);
}

void GLES3Swapchain::doDestroy() {
    GLES3Device::getInstance()->onSwapchainDestroyed(this);

    CC_SAFE_DESTROY(_colorTexture);
    CC_SAFE_DESTROY(_depthStencilTexture);
    CC_SAFE_DESTROY(_context);
}

void GLES3Swapchain::doResize(uint32_t width, uint32_t height) {
}

void GLES3Swapchain::doDestroySurface() {
    _context->releaseSurface();
}

void GLES3Swapchain::doCreateSurface(void* windowHandle) {
    _context->acquireSurface(reinterpret_cast<uintptr_t>(windowHandle));
}

} // namespace gfx
} // namespace cc
