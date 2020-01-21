#include "MTLStd.h"
#include "MTLWindow.h"

NS_CC_BEGIN

CCMTLWindow::CCMTLWindow(GFXDevice* device) : GFXWindow(device) {}
CCMTLWindow::~CCMTLWindow() { destroy(); }

bool CCMTLWindow::initialize(const GFXWindowInfo& info)
{
    _title = info.title;
    _left = info.left;
    _top = info.top;
    _width = info.width;
    _height = info.height;
    _nativeWidth = _width;
    _nativeHeight = _height;
    _colorFmt = info.color_fmt;
    _depthStencilFmt = info.depth_stencil_fmt;
    _isOffscreen = info.is_offscreen;
    _isFullscreen = info.is_fullscreen;
    
    GFXRenderPassInfo render_pass_info;
    
    GFXColorAttachment color_attachment;
    color_attachment.format = GFXFormat::BGRA8UN;
    color_attachment.load_op = GFXLoadOp::CLEAR;
    color_attachment.store_op = GFXStoreOp::STORE;
    color_attachment.sample_count = 1;
    color_attachment.begin_layout = GFXTextureLayout::COLOR_ATTACHMENT_OPTIMAL;
    color_attachment.end_layout = GFXTextureLayout::COLOR_ATTACHMENT_OPTIMAL;
    render_pass_info.color_attachments.emplace_back(color_attachment);
    
    GFXDepthStencilAttachment& depth_stencil_attachment = render_pass_info.depth_stencil_attachment;
    render_pass_info.depth_stencil_attachment.format = GFXFormat::D24S8;
    depth_stencil_attachment.depth_load_op = GFXLoadOp::DISCARD;
    depth_stencil_attachment.depth_store_op = GFXStoreOp::DISCARD;
    depth_stencil_attachment.stencil_load_op = GFXLoadOp::DISCARD;
    depth_stencil_attachment.stencil_store_op = GFXStoreOp::DISCARD;
    depth_stencil_attachment.sample_count = 1;
    depth_stencil_attachment.begin_layout = GFXTextureLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    depth_stencil_attachment.end_layout = GFXTextureLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    
    _renderPass = _device->createRenderPass(render_pass_info);
    
    // Create texture & texture views
    if (_isOffscreen) {
        if (_colorFmt != GFXFormat::UNKNOWN) {
            GFXTextureInfo color_tex_info;
            color_tex_info.type = GFXTextureType::TEX2D;
            color_tex_info.usage = GFXTextureUsageBit::COLOR_ATTACHMENT | GFXTextureUsageBit::SAMPLED;
            color_tex_info.format = _colorFmt;
            color_tex_info.width = _width;
            color_tex_info.height = _height;
            color_tex_info.depth = 1;
            color_tex_info.array_layer = 1;
            color_tex_info.mip_level = 1;
            color_texture_ = _device->createTexture(color_tex_info);
            
            GFXTextureViewInfo color_tex_view_info;
            color_tex_view_info.type = GFXTextureViewType::TV2D;
            color_tex_view_info.format = _colorFmt;
            color_tex_view_info.base_level = 0;
            color_tex_view_info.level_count = 1;
            color_tex_view_info.base_layer = 0;
            color_tex_view_info.layer_count = 1;
            color_tex_view_ = _device->createTextureView(color_tex_view_info);
        }
        if (_depthStencilFmt != GFXFormat::UNKNOWN) {
            GFXTextureInfo depth_stecnil_tex_info;
            depth_stecnil_tex_info.type = GFXTextureType::TEX2D;
            depth_stecnil_tex_info.usage = GFXTextureUsageBit::DEPTH_STENCIL_ATTACHMENT | GFXTextureUsageBit::SAMPLED;
            depth_stecnil_tex_info.format = _depthStencilFmt;
            depth_stecnil_tex_info.width = _width;
            depth_stecnil_tex_info.height = _height;
            depth_stecnil_tex_info.depth = 1;
            depth_stecnil_tex_info.array_layer = 1;
            depth_stecnil_tex_info.mip_level = 1;
            depth_stencil_texture_ = _device->createTexture(depth_stecnil_tex_info);
            
            GFXTextureViewInfo depth_stecnil_tex_view_info;
            depth_stecnil_tex_view_info.type = GFXTextureViewType::TV2D;
            depth_stecnil_tex_view_info.format = _colorFmt;
            depth_stecnil_tex_view_info.base_level = 0;
            depth_stecnil_tex_view_info.level_count = 1;
            depth_stecnil_tex_view_info.base_layer = 0;
            depth_stecnil_tex_view_info.layer_count = 1;
            depth_stencil_tex_view_ = _device->createTextureView(depth_stecnil_tex_view_info);
        }
    }

    GFXFramebufferInfo fbo_info;
    fbo_info.render_pass = _renderPass;
    fbo_info.color_views.push_back(color_tex_view_);
    fbo_info.depth_stencil_view = depth_stencil_tex_view_;
    fbo_info.is_offscreen = _isOffscreen;
    framebuffer_ = _device->createFramebuffer(fbo_info);
    
    return true;
}

void CCMTLWindow::destroy()
{
    CC_SAFE_DESTROY(_renderPass);
    CC_SAFE_DESTROY(color_tex_view_);
    CC_SAFE_DESTROY(color_texture_);
    CC_SAFE_DESTROY(depth_stencil_tex_view_);
    CC_SAFE_DESTROY(depth_stencil_texture_);
    CC_SAFE_DESTROY(framebuffer_);
}

void CCMTLWindow::resize(uint width, uint height)
{
    
}

NS_CC_END
