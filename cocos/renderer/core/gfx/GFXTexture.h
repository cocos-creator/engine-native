#ifndef CC_CORE_GFX_TEXTURE_H_
#define CC_CORE_GFX_TEXTURE_H_

#include "GFXDef.h"

NS_CC_BEGIN

class CC_CORE_API GFXTexture : public Object {
 public:
  GFXTexture(GFXDevice* device);
  virtual ~GFXTexture();
  
 public:
  virtual bool initialize(const GFXTextureInfo& info) = 0;
  virtual void destroy() = 0;
  virtual void resize(uint width, uint height) = 0;
  
  CC_INLINE GFXTextureType type() const { return _type; }
  CC_INLINE GFXTextureUsage usage() const { return _usage; }
  CC_INLINE GFXFormat format() const { return format_; }
  CC_INLINE uint width() const { return _width; }
  CC_INLINE uint height() const { return _height; }
  CC_INLINE uint depth() const { return depth_; }
  CC_INLINE uint array_layer() const { return array_layer_; }
  CC_INLINE uint mip_level() const { return mip_level_; }
  CC_INLINE uint size() const { return _size; }
  CC_INLINE GFXSampleCount samples() const { return samples_; }
  CC_INLINE GFXTextureFlags flags() const { return _flags; }
  CC_INLINE uint8_t* buffer() const { return _buffer; }
  
 protected:
  GFXDevice* _device;
  GFXTextureType _type;
  GFXTextureUsage _usage;
  GFXFormat format_;
  uint _width;
  uint _height;
  uint depth_;
  uint array_layer_;
  uint mip_level_;
  uint _size;
  GFXSampleCount samples_;
  GFXTextureFlags _flags;
  uint8_t* _buffer;
};

NS_CC_END

#endif // CC_CORE_GFX_TEXTURE_H_
