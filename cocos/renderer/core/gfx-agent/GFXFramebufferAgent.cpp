#include "CoreStd.h"

#include "threading/MessageQueue.h"
#include "GFXDeviceAgent.h"
#include "GFXFramebufferAgent.h"
#include "GFXRenderPassAgent.h"
#include "GFXTextureAgent.h"

namespace cc {
namespace gfx {

bool FramebufferAgent::initialize(const FramebufferInfo &info) {
    _renderPass = info.renderPass;
    _colorTextures = info.colorTextures;
    _depthStencilTexture = info.depthStencilTexture;

    FramebufferInfo actorInfo = info;
    for (uint i = 0u; i < info.colorTextures.size(); ++i) {
        if (info.colorTextures[i]) {
            actorInfo.colorTextures[i] = ((TextureAgent *)info.colorTextures[i])->getActor();
        }
    }
    if (info.depthStencilTexture) {
        actorInfo.depthStencilTexture = ((TextureAgent *)info.depthStencilTexture)->getActor();
    }
    actorInfo.renderPass = ((RenderPassAgent *)info.renderPass)->getActor();

    ENQUEUE_MESSAGE_2(
        ((DeviceAgent *)_device)->getMessageQueue(),
        FramebufferInit,
        actor, getActor(),
        info, actorInfo,
        {
            actor->initialize(info);
        });

    return true;
}

void FramebufferAgent::destroy() {
    if (_actor) {
        ENQUEUE_MESSAGE_1(
            ((DeviceAgent *)_device)->getMessageQueue(),
            FramebufferDestroy,
            actor, getActor(),
            {
                CC_DESTROY(actor);
            });

        _actor = nullptr;
    }
}

} // namespace gfx
} // namespace cc
