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

#pragma once

#include "GFXTexture.h"
#include "gfx-base/GFXDef-common.h"

namespace cc {
namespace gfx {

class CC_DLL Swapchain : public GFXObject {
public:
    Swapchain();
    ~Swapchain() override;

    void initialize(const SwapchainInfo &info);
    void resize(uint32_t width, uint32_t height);
    void destroy();

    inline void destroySurface();
    inline void createSurface(void *windowHandle);

    inline void *    getWindowHandle() { return _windowHandle; }
    inline VsyncMode getVSyncMode() { return _vsyncMode; }

    inline uint32_t getWidth() { return _colorTexture->getWidth(); }
    inline uint32_t getHeight() { return _colorTexture->getHeight(); }

    inline Texture *getColorTexture() { return _colorTexture; }
    inline Texture *getDepthStencilTexture() { return _depthStencilTexture; }

    virtual SurfaceTransform getSurfaceTransform() const { return _transform; }

protected:
    virtual void doInit(const SwapchainInfo &info)         = 0;
    virtual void doDestroy()                               = 0;
    virtual void doResize(uint32_t width, uint32_t height) = 0;
    virtual void doDestroySurface()                        = 0;
    virtual void doCreateSurface(void *windowHandle)       = 0;

    static inline void initTexture(const SwapchainTextureInfo &info, Texture *texture);

    void *           _windowHandle{nullptr};
    VsyncMode        _vsyncMode{VsyncMode::RELAXED};
    SurfaceTransform _transform{SurfaceTransform::IDENTITY};

    Texture *_colorTexture{nullptr};
    Texture *_depthStencilTexture{nullptr};
};

///////////////////////////////////////////////////////////

void Swapchain::destroySurface() {
    doDestroySurface();
    _windowHandle = nullptr;
}

void Swapchain::createSurface(void *windowHandle) {
    _windowHandle = windowHandle;
    doCreateSurface(windowHandle);
}

void Swapchain::initTexture(const SwapchainTextureInfo &info, Texture *texture) {
    Texture::initialize(info, texture);
}

} // namespace gfx
} // namespace cc
