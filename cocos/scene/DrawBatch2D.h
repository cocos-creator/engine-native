/****************************************************************************
 Copyright (c) 2021 Xiamen Yaji Software Co., Ltd.
 
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

#include <vector>
#include "renderer/pipeline/helper/SharedMemory.h"
#include "renderer/gfx-base/GFXDescriptorSet.h"
#include "renderer/gfx-base/GFXInputAssembler.h"

namespace cc {

namespace scene {

class Pass;

struct DrawBatch2DLayout final {
    uint32_t visFlags{0};
    uint32_t passCount{0};
    uint32_t descriptorSet{0U};
    uint32_t inputAssembler{0U};
    uint32_t passes[8];
    uint32_t shaders[8];
};

class DrawBatch2D final {
public:
    DrawBatch2D()                    = default;
    DrawBatch2D(const DrawBatch2D &) = delete;
    DrawBatch2D(DrawBatch2D &&)      = delete;
    ~DrawBatch2D()                   = default;
    DrawBatch2D &operator=(const DrawBatch2D &) = delete;
    DrawBatch2D &operator=(DrawBatch2D &&) = delete;

    inline uint32_t             getVisFlags() const { return static_cast<uint32_t>(_batchLayout->visFlags); }
    inline gfx::DescriptorSet * getDescriptorSet() const { return GET_DESCRIPTOR_SET(_batchLayout->descriptorSet); }
    inline gfx::InputAssembler *getInputAssembler() const { return GET_IA(_batchLayout->inputAssembler); }
    inline uint8_t              getPassesCount() const { return _batchLayout->passCount; }
    inline Pass *               getPass(uint8_t index) const { return GET_PASS(_batchLayout->passes[index]);}
    inline gfx::Shader *getShader(uint8_t index) const { return GET_SHADER(_batchLayout->shaders[index]); }

    void initWithData(uint8_t *data);

private:
    DrawBatch2DLayout *_batchLayout{nullptr};
};

} // namespace scene
} // namespace cc
