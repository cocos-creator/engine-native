#ifndef CC_GFXGLES3_GLES3_COMMAND_ALLOCATOR_H_
#define CC_GFXGLES3_GLES3_COMMAND_ALLOCATOR_H_

#include "gfx/GFXCommandPool.h"
#include "GLES3Commands.h"

NS_CC_BEGIN

class CC_GLES3_API GLES3CommandAllocator : public GFXCommandAllocator {
public:
  GLES3CommandAllocator(GFXDevice* device);
  ~GLES3CommandAllocator();
  
  GFXCommandPool<GLES3CmdBeginRenderPass> beginRenderPassCmdPool;
  GFXCommandPool<GLES3CmdBindStates> bindStatesCmdPool;
  GFXCommandPool<GLES3CmdDraw> drawCmdPool;
  GFXCommandPool<GLES3CmdUpdateBuffer> updateBufferCmdPool;
  GFXCommandPool<GLES3CmdCopyBufferToTexture> copyBufferToTextureCmdPool;
  
public:
  bool Initialize(const GFXCommandAllocatorInfo& info);
  void Destroy();
  
  void clearCmds(GLES3CmdPackage* cmd_package);
  
  CC_INLINE void releaseCmds() {
    beginRenderPassCmdPool.Release();
    bindStatesCmdPool.Release();
    drawCmdPool.Release();
    updateBufferCmdPool.Release();
    copyBufferToTextureCmdPool.Release();
  }
};

NS_CC_END

#endif
