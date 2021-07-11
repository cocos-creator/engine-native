/****************************************************************************
 Copyright (c) 2020-2021 Xiamen Yaji Software Co., Ltd.

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

#include "base/CoreStd.h"
#include "base/threading/MessageQueue.h"

#include "DeviceValidator.h"
#include "PipelineLayoutValidator.h"
#include "PipelineStateValidator.h"
#include "RenderPassValidator.h"
#include "ShaderValidator.h"
#include "ValidationUtils.h"

namespace cc {
namespace gfx {

PipelineStateValidator::PipelineStateValidator(PipelineState *actor)
: Agent<PipelineState>(actor) {
    _typedID = generateObjectID<decltype(this)>();
}

PipelineStateValidator::~PipelineStateValidator() {
    DeviceResourceTracker<PipelineState>::erase(this);
    CC_SAFE_DELETE(_actor);
}

void PipelineStateValidator::doInit(const PipelineStateInfo &info) {
    PipelineStateInfo actorInfo = info;
    actorInfo.shader            = static_cast<ShaderValidator *>(info.shader)->getActor();
    actorInfo.pipelineLayout    = static_cast<PipelineLayoutValidator *>(info.pipelineLayout)->getActor();
    if (info.renderPass) {
        actorInfo.renderPass = static_cast<RenderPassValidator *>(info.renderPass)->getActor();

        const auto &subpasses            = info.renderPass->getSubpasses();
        size_t      colorAttachmentCount = info.subpass >= subpasses.size()
                                               ? info.renderPass->getColorAttachments().size()
                                               : subpasses[info.subpass].colors.size();
        //CCASSERT(colorAttachmentCount == info.blendState.targets.size(), "Wrong number of blend targets"); // be more lenient on this
    }

    _actor->initialize(actorInfo);
}

void PipelineStateValidator::doDestroy() {
    _actor->destroy();
}

} // namespace gfx
} // namespace cc
