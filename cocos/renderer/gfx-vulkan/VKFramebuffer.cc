#include "VKStd.h"
#include "VKFramebuffer.h"
#include "VKRenderPass.h"
#include "VKCommands.h"
#include "VKTextureView.h"
#include "VKDevice.h"

NS_CC_BEGIN

CCVKFramebuffer::CCVKFramebuffer(GFXDevice* device)
    : GFXFramebuffer(device)
{
}

CCVKFramebuffer::~CCVKFramebuffer()
{
}

bool CCVKFramebuffer::initialize(const GFXFramebufferInfo &info)
{
    _renderPass = info.renderPass;
    _colorViews = info.colorViews;
    _depthStencilView = info.depthStencilView;
    _isOffscreen = info.isOffscreen;

    _gpuFBO = CC_NEW(CCVKGPUFramebuffer);
    _gpuFBO->gpuRenderPass = ((CCVKRenderPass*)_renderPass)->gpuRenderPass();
    _gpuFBO->isOffscreen = _isOffscreen;

    _gpuFBO->gpuColorViews.resize(_colorViews.size());
    for (size_t i = 0; i < _colorViews.size(); ++i)
    {
        CCVKTextureView* color_view = (CCVKTextureView*)_colorViews[i];
        _gpuFBO->gpuColorViews[i] = color_view->gpuTexView();
    }

    if (_depthStencilView)
    {
        //_gpuFBO->gpuDepthStencilView = ((CCVKTextureView*)_depthStencilView)->gpuTexView();
    }

    CCVKCmdFuncCreateFramebuffer((CCVKDevice*)_device, _gpuFBO);

    _status = GFXStatus::SUCCESS;

    return true;
}

void CCVKFramebuffer::destroy()
{
    if (_gpuFBO)
    {
        CCVKCmdFuncDestroyFramebuffer((CCVKDevice*)_device, _gpuFBO);
        CC_DELETE(_gpuFBO);
        _gpuFBO = nullptr;
    }

    _status = GFXStatus::UNREADY;
}

NS_CC_END
