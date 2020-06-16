#include "MTLStd.h"
#include "MTLFence.h"
#include "MTLDevice.h"

namespace cc {

CCMTLFence::CCMTLFence(GFXDevice *device) : GFXFence(device) {}

CCMTLFence::~CCMTLFence() {
    destroy();
}

bool CCMTLFence::initialize(const GFXFenceInfo &info) {
    // TODO

    _status = GFXStatus::SUCCESS;
    
    return true;
}

void CCMTLFence::destroy() {
    // TODO
    _status = GFXStatus::UNREADY;
}

void CCMTLFence::wait() {
    // TODO
}

void CCMTLFence::reset() {
    // TODO
}

}

