/****************************************************************************
Copyright (c) 2020 Xiamen Yaji Software Co., Ltd.

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/
#include "GLES3Std.h"
#include "GLES3Queue.h"
#include "GLES3Commands.h"
#include "GLES3CommandBuffer.h"

namespace cc {
namespace gfx {

GLES3Queue::GLES3Queue(Device *device)
: Queue(device) {
}

GLES3Queue::~GLES3Queue() {
}

bool GLES3Queue::initialize(const QueueInfo &info) {
    _type = info.type;

    return true;
}

void GLES3Queue::destroy() {
}

void GLES3Queue::submit(const CommandBuffer *const *cmdBuffs, uint count, Fence *fence) {
    if (!_isAsync) {
        for (uint i = 0; i < count; ++i) {
            GLES3CommandBuffer *cmdBuff = (GLES3CommandBuffer *)cmdBuffs[i];
            GLES3CmdPackage *cmdPackage = cmdBuff->_pendingPackages.front();

            GLES3CmdFuncExecuteCmds((GLES3Device *)_device, cmdPackage);

            _numDrawCalls += cmdBuff->_numDrawCalls;
            _numInstances += cmdBuff->_numInstances;
            _numTriangles += cmdBuff->_numTriangles;

            cmdBuff->_pendingPackages.pop();
            cmdBuff->_freePackages.push(cmdPackage);
            cmdBuff->_cmdAllocator->clearCmds(cmdPackage);
            cmdBuff->_cmdAllocator->reset();
        }
    }
}

} // namespace gfx
} // namespace cc
