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

#pragma once
#include "renderer/gfx-base/GFXDef.h"
#include "bindings/dop/ObjectPool.h"
#include "bindings/dop/PoolType.h"

namespace cc {
namespace gfx {
class DescriptorSet;
}

namespace pipeline {

//Get object pool data
#define GET_PASS(index)                pipeline::SharedMemory::getObject<scene::Pass, se::ObjectPoolType::PASS>(index)
#define GET_DESCRIPTOR_SET(index)      pipeline::SharedMemory::getObject<gfx::DescriptorSet, se::ObjectPoolType::DESCRIPTOR_SET>(index)
#define GET_IA(index)                  pipeline::SharedMemory::getObject<gfx::InputAssembler, se::ObjectPoolType::INPUT_ASSEMBLER>(index)
#define GET_SHADER(index)              pipeline::SharedMemory::getObject<gfx::Shader, se::ObjectPoolType::SHADER>(index)
#define GET_BLEND_STATE(index)         pipeline::SharedMemory::getObject<gfx::BlendState, se::ObjectPoolType::BLEND_STATE>(index)
#define GET_DEPTH_STENCIL_STATE(index) pipeline::SharedMemory::getObject<gfx::DepthStencilState, se::ObjectPoolType::DEPTH_STENCIL_STATE>(index)
#define GET_RASTERIZER_STATE(index)    pipeline::SharedMemory::getObject<gfx::RasterizerState, se::ObjectPoolType::RASTERIZER_STATE>(index)

class CC_DLL SharedMemory : public Object {
public:
    template <typename T, se::ObjectPoolType p>
    static T *getObject(uint index) {
        const auto &poolMap = se::ObjectPool::getPoolMap();
        const auto  type    = GET_OBJECT_POOL_ID(p);

#ifdef CC_DEBUG
        CCASSERT(poolMap[type] != nullptr, "BufferPool: Invalid buffer pool type");
#endif

        const se::ObjectPool *objectPool = poolMap[type];
        return objectPool->getTypedObject<T>(index);
    }
};

} //namespace pipeline
} //namespace cc
