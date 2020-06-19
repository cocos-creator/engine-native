#include "MTLStd.h"
#include "MTLBindingLayout.h"

namespace cc {
namespace gfx {

CCMTLBindingLayout::CCMTLBindingLayout(Device *device) : GFXBindingLayout(device) {}
CCMTLBindingLayout::~CCMTLBindingLayout() { destroy(); }

bool CCMTLBindingLayout::initialize(const GFXBindingLayoutInfo &info) {
    if (info.bindings.size()) {
        _bindingUnits.resize(info.bindings.size());
        for (size_t i = 0; i < _bindingUnits.size(); ++i) {
            GFXBindingUnit &bindingUnit = _bindingUnits[i];
            const GFXBinding &binding = info.bindings[i];
            bindingUnit.shaderStages = binding.shaderStages;
            bindingUnit.binding = binding.binding;
            bindingUnit.type = binding.type;
            bindingUnit.name = binding.name;
        }
    }

    _status = GFXStatus::SUCCESS;
    return true;
}

void CCMTLBindingLayout::destroy() {
    _status = GFXStatus::UNREADY;
}

void CCMTLBindingLayout::update() {
}

} // namespace gfx
} // namespace cc
