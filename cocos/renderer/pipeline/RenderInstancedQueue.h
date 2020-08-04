#pragma once

#include "core/CoreStd.h"

namespace cc {
namespace pipeline {

class InstancedBuffer;

class CC_DLL RenderInstancedQueue : public Object {
public:
    RenderInstancedQueue() = default;
    ~RenderInstancedQueue() = default;

    void clear();
    void recordCommandBuffer(gfx::Device *, gfx::RenderPass *, gfx::CommandBuffer *);

    CC_INLINE const set<InstancedBuffer *> &getQueue() const { return _queue; }

private:
    set<InstancedBuffer *> _queue;
};

} // namespace pipeline
} // namespace cc