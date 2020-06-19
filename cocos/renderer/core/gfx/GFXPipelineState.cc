#include "CoreStd.h"
#include "GFXPipelineState.h"

namespace cc {
namespace gfx {

GFXPipelineState::GFXPipelineState(Device *device)
: GFXObject(GFXObjectType::PIPELINE_STATE), _device(device) {
}

GFXPipelineState::~GFXPipelineState() {
}

} // namespace gfx
} // namespace cc
