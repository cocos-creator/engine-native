#include "platform/CCCanvasRenderingContext2D.h"
#include "base/ccTypes.h"
#include "base/csscolorparser.hpp"

#include "cocos/scripting/js-bindings/jswrapper/SeApi.h"

#include <regex>

using namespace cocos2d;

enum class CanvasTextAlign {
    LEFT,
    CENTER,
    RIGHT
};

enum class CanvasTextBaseline {
    TOP,
    MIDDLE,
    BOTTOM
};

namespace {
  void fillRectWithColor(uint8_t* buf, uint32_t totalWidth, uint32_t totalHeight, uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint8_t r, uint8_t g, uint8_t b)
  {
    assert(x + width <= totalWidth);
    assert(y + height <= totalHeight);

    uint32_t y0 = totalHeight - (y + height);
    uint32_t y1 = totalHeight - y;
    uint8_t* p;
    for (uint32_t offsetY = y0; offsetY < y1; ++offsetY)
    {
      for (uint32_t offsetX = x; offsetX < (x + width); ++offsetX)
      {
        p = buf + (totalWidth * offsetY + offsetX) * 3;
        *p++ = r;
        *p++ = g;
        *p++ = b;
      }
    }
  }
}

class CanvasRenderingContext2DImpl
{
public:
  CanvasRenderingContext2DImpl(HWND hWnd = nullptr) : _DC(nullptr)
    , _bmp(nullptr)
    , _font((HFONT)GetStockObject(DEFAULT_GUI_FONT))
    , _wnd(nullptr)
    , _savedDC(0)
    {
      _wnd = hWnd;
      HDC hdc = GetDC(_wnd);
      _DC = CreateCompatibleDC(hdc);
      ReleaseDC(_wnd, hdc);
    }

    ~CanvasRenderingContext2DImpl()
    {
      if (_DC)
      {
        DeleteDC(_DC);
      }
    }

    void recreateBuffer(float w, float h)
    {
        _bufferWidth = w;
        _bufferHeight = h;
        if (_bufferWidth < 1.0f || _bufferHeight < 1.0f)
            return;

        int textureSize = _bufferWidth * _bufferHeight * 4;
        uint8_t* data = (uint8_t*)malloc(sizeof(uint8_t) * textureSize);
        memset(data, 0, textureSize);
        _imageData.fastSet(data, textureSize);
        // todo, create bitmap use data above
    }

    void beginPath()
    {
      _DC = BeginPaint(_wnd, &_paintStruct);
    }

    void closePath()
    {
      EndPaint(_wnd, &_paintStruct);
    }

    void moveTo(float x, float y)
    {
      MoveToEx(_DC, x, y, nullptr);
    }

    void lineTo(float x, float y)
    {
      LineTo(_DC, x, y);
    }

    void stroke()
    {
        if (_bufferWidth < 1.0f || _bufferHeight < 1.0f)
            return;
        //
        fillData();
    }

    void saveContext()
    {
      _savedDC = SaveDC(_DC);
    }

    void restoreContext()
    {
      RestoreDC(_DC, _savedDC);
    }

    void clearRect(float x, float y, float w, float h)
    {
        if (_bufferWidth < 1.0f || _bufferHeight < 1.0f)
            return;
        if (_imageData.isNull())
          return;

        uint8_t* buffer = _imageData.getBytes();
        if (buffer)
        {
          uint8_t r = 0;
          uint8_t g = 0;
          uint8_t b = 0;
          fillRectWithColor(buffer, (uint32_t)_bufferWidth, (uint32_t)_bufferHeight, (uint32_t)x, (uint32_t)y, (uint32_t)w, (uint32_t)h, r, g, b);
        }
    }

    void fillRect(float x, float y, float w, float h)
    {
        if (_bufferWidth < 1.0f || _bufferHeight < 1.0f)
            return;

        uint8_t* buffer = _imageData.getBytes();
        if (buffer)
        {
          uint8_t r = _fillStyle.r * 255.0f;
          uint8_t g = _fillStyle.g * 255.0f;
          uint8_t b = _fillStyle.b * 255.0f;
          fillRectWithColor(buffer, (uint32_t)_bufferWidth, (uint32_t)_bufferHeight, (uint32_t)x, (uint32_t)y, (uint32_t)w, (uint32_t)h, r, g, b);
        }
    }

    void fillText(const std::string& text, float x, float y, float maxWidth)
    {
        if (text.empty() || _bufferWidth < 1.0f || _bufferHeight < 1.0f)
            return;
        //
        fillData();
    }

    void strokeText(const std::string& text, float x, float y, float maxWidth)
    {
        if (text.empty() || _bufferWidth < 1.0f || _bufferHeight < 1.0f)
            return;
        //
        fillData();
    }

    float measureText(const std::string& text)
    {
        if (text.empty())
            return 0.0f;
        //
    }

    void updateFont(const std::string& fontName, float fontSize, bool bold)
    {
    }

    void setTextAlign(CanvasTextAlign align)
    {
    }

    void setTextBaseline(CanvasTextBaseline baseline)
    {
    }

    void setFillStyle(float r, float g, float b, float a)
    {
        _fillStyle.r = r;
        _fillStyle.g = g;
        _fillStyle.b = b;
        _fillStyle.a = a;
    }

    void setStrokeStyle(float r, float g, float b, float a)
    {
        _strokeStyle.r = r;
        _strokeStyle.g = g;
        _strokeStyle.b = b;
        _strokeStyle.a = a;
    }

    void setLineWidth(float lineWidth)
    {
    }

    const Data& getDataRef() const
    {
        return _imageData;
    }

    void fillData()
    {
      BITMAP bitmap;
      GetObject(_bmp, sizeof(BITMAP), &bitmap);
      // check, bmBits == null?
      _imageData.fastSet((uint8_t*)bitmap.bmBits, sizeof(bitmap.bmBits));
    }

    HDC _DC;
    HBITMAP _bmp;
private:

    Data _imageData;
    HFONT   _font;
    HWND    _wnd;
    PAINTSTRUCT _paintStruct;
    std::string _curFontPath;
    int _savedDC;
    float _bufferWidth = 0.0f;
    float _bufferHeight = 0.0f;

    CanvasTextAlign _textAlign;
    CanvasTextBaseline _textBaseLine;
    cocos2d::Color4F _fillStyle;
    cocos2d::Color4F _strokeStyle;
};

NS_CC_BEGIN

CanvasGradient::CanvasGradient()
{
    // SE_LOGD("CanvasGradient constructor: %p\n", this);
}

CanvasGradient::~CanvasGradient()
{
    // SE_LOGD("CanvasGradient destructor: %p\n", this);
}

void CanvasGradient::addColorStop(float offset, const std::string& color)
{
    // SE_LOGD("CanvasGradient::addColorStop: %p\n", this);
}

// CanvasRenderingContext2D

CanvasRenderingContext2D::CanvasRenderingContext2D(float width, float height)
: __width(width)
, __height(height)
{
    // SE_LOGD("CanvasRenderingContext2D constructor: %p, width: %f, height: %f\n", this, width, height);
    _impl = new CanvasRenderingContext2DImpl();
    recreateBufferIfNeeded();
}

CanvasRenderingContext2D::~CanvasRenderingContext2D()
{
    // SE_LOGD("CanvasRenderingContext2D destructor: %p\n", this);
    delete _impl;
}

void CanvasRenderingContext2D::recreateBufferIfNeeded()
{
    if (_isBufferSizeDirty)
    {
        _isBufferSizeDirty = false;
//        SE_LOGD("Recreate buffer %p, w: %f, h:%f\n", this, __width, __height);
        _impl->recreateBuffer(__width, __height);
        if (_canvasBufferUpdatedCB != nullptr)
            _canvasBufferUpdatedCB(_impl->getDataRef());
    }
}

void CanvasRenderingContext2D::clearRect(float x, float y, float width, float height)
{
//    SE_LOGD("CanvasRenderingContext2D::clearRect: %p, %f, %f, %f, %f\n", this, x, y, width, height);
    recreateBufferIfNeeded();
    _impl->clearRect(x, y, width, height);
}

void CanvasRenderingContext2D::fillRect(float x, float y, float width, float height)
{
    recreateBufferIfNeeded();
    _impl->fillRect(x, y, width, height);

    if (_canvasBufferUpdatedCB != nullptr)
        _canvasBufferUpdatedCB(_impl->getDataRef());
}

void CanvasRenderingContext2D::fillText(const std::string& text, float x, float y, float maxWidth)
{
//    SE_LOGD("CanvasRenderingContext2D::fillText: %s, %f, %f, %f\n", text.c_str(), x, y, maxWidth);
    if (text.empty())
        return;
    recreateBufferIfNeeded();

    _impl->fillText(text, x, y, maxWidth);
    if (_canvasBufferUpdatedCB != nullptr)
        _canvasBufferUpdatedCB(_impl->getDataRef());
}

void CanvasRenderingContext2D::strokeText(const std::string& text, float x, float y, float maxWidth)
{
//    SE_LOGD("CanvasRenderingContext2D::strokeText: %s, %f, %f, %f\n", text.c_str(), x, y, maxWidth);
    if (text.empty())
        return;
    recreateBufferIfNeeded();

    _impl->strokeText(text, x, y, maxWidth);

   if (_canvasBufferUpdatedCB != nullptr)
       _canvasBufferUpdatedCB(_impl->getDataRef());
}

cocos2d::Size CanvasRenderingContext2D::measureText(const std::string& text)
{
//    SE_LOGD("CanvasRenderingContext2D::measureText: %s\n", text.c_str());
    return cocos2d::Size(_impl->measureText(text), 0);
}

CanvasGradient* CanvasRenderingContext2D::createLinearGradient(float x0, float y0, float x1, float y1)
{
    return nullptr;
}

void CanvasRenderingContext2D::save()
{
    _impl->saveContext();
}

void CanvasRenderingContext2D::beginPath()
{
    _impl->beginPath();
}

void CanvasRenderingContext2D::closePath()
{
    _impl->closePath();
}

void CanvasRenderingContext2D::moveTo(float x, float y)
{
    _impl->moveTo(x, y);
}

void CanvasRenderingContext2D::lineTo(float x, float y)
{
    _impl->lineTo(x, y);
}

void CanvasRenderingContext2D::stroke()
{
    _impl->stroke();

    if (_canvasBufferUpdatedCB != nullptr)
        _canvasBufferUpdatedCB(_impl->getDataRef());
}

void CanvasRenderingContext2D::restore()
{
    _impl->restoreContext();
}

void CanvasRenderingContext2D::setCanvasBufferUpdatedCallback(const CanvasBufferUpdatedCallback& cb)
{
    _canvasBufferUpdatedCB = cb;
}

void CanvasRenderingContext2D::set__width(float width)
{
//    SE_LOGD("CanvasRenderingContext2D::set__width: %f\n", width);
    __width = width;
    _isBufferSizeDirty = true;
}

void CanvasRenderingContext2D::set__height(float height)
{
//    SE_LOGD("CanvasRenderingContext2D::set__height: %f\n", height);
    __height = height;
    _isBufferSizeDirty = true;
}

void CanvasRenderingContext2D::set_lineWidth(float lineWidth)
{
    _lineWidth = lineWidth;
    _impl->setLineWidth(lineWidth);
}

void CanvasRenderingContext2D::set_lineJoin(const std::string& lineJoin)
{
    // SE_LOGE("%s isn't implemented!\n", __FUNCTION__);
}

void CanvasRenderingContext2D::set_font(const std::string& font)
{
    if (_font != font)
    {
        _font = font;

        std::string boldStr;
        std::string fontName = "Arial";
        std::string fontSizeStr = "30";

        std::regex re("(bold)?\\s*(\\d+)px\\s+(\\w+)");
        std::match_results<std::string::const_iterator> results;
        if (std::regex_search(_font.cbegin(), _font.cend(), results, re))
        {
            boldStr = results[1].str();
            fontSizeStr = results[2].str();
            fontName = results[3].str();
        }

        float fontSize = atof(fontSizeStr.c_str());
        _impl->updateFont(fontName, fontSize, !boldStr.empty());
    }
}

void CanvasRenderingContext2D::set_textAlign(const std::string& textAlign)
{
    // SE_LOGD("CanvasRenderingContext2D::set_textAlign: %s\n", textAlign.c_str());
    if (textAlign == "left")
    {
        _impl->setTextAlign(CanvasTextAlign::LEFT);
    }
    else if (textAlign == "center" || textAlign == "middle")
    {
        _impl->setTextAlign(CanvasTextAlign::CENTER);
    }
    else if (textAlign == "right")
    {
        _impl->setTextAlign(CanvasTextAlign::RIGHT);
    }
    else
    {
        assert(false);
    }
}

void CanvasRenderingContext2D::set_textBaseline(const std::string& textBaseline)
{
    // SE_LOGD("CanvasRenderingContext2D::set_textBaseline: %s\n", textBaseline.c_str());
    if (textBaseline == "top")
    {
        _impl->setTextBaseline(CanvasTextBaseline::TOP);
    }
    else if (textBaseline == "middle")
    {
        _impl->setTextBaseline(CanvasTextBaseline::MIDDLE);
    }
    else if (textBaseline == "bottom" || textBaseline == "alphabetic") //TODO:cjh, how to deal with alphabetic, currently we handle it as bottom mode.
    {
        _impl->setTextBaseline(CanvasTextBaseline::BOTTOM);
    }
    else
    {
        assert(false);
    }
}

void CanvasRenderingContext2D::set_fillStyle(const std::string& fillStyle)
{
    CSSColorParser::Color color = CSSColorParser::parse(fillStyle);
    _impl->setFillStyle(color.r/255.0f, color.g/255.0f, color.b/255.0f, color.a);
    // SE_LOGD("CanvasRenderingContext2D::set_fillStyle: %s, (%d, %d, %d, %f)\n", fillStyle.c_str(), color.r, color.g, color.b, color.a);
}

void CanvasRenderingContext2D::set_strokeStyle(const std::string& strokeStyle)
{
    CSSColorParser::Color color = CSSColorParser::parse(strokeStyle);
    _impl->setStrokeStyle(color.r/255.0f, color.g/255.0f, color.b/255.0f, color.a);
}

void CanvasRenderingContext2D::set_globalCompositeOperation(const std::string& globalCompositeOperation)
{
    // SE_LOGE("%s isn't implemented!\n", __FUNCTION__);
}

// transform
//TODO:

void CanvasRenderingContext2D::translate(float x, float y)
{
    // SE_LOGE("%s isn't implemented!\n", __FUNCTION__);
}

void CanvasRenderingContext2D::scale(float x, float y)
{
    // SE_LOGE("%s isn't implemented!\n", __FUNCTION__);
}

void CanvasRenderingContext2D::rotate(float angle)
{
    // SE_LOGE("%s isn't implemented!\n", __FUNCTION__);
}

void CanvasRenderingContext2D::transform(float a, float b, float c, float d, float e, float f)
{
    // SE_LOGE("%s isn't implemented!\n", __FUNCTION__);
}

void CanvasRenderingContext2D::setTransform(float a, float b, float c, float d, float e, float f)
{
    // SE_LOGE("%s isn't implemented!\n", __FUNCTION__);
}

NS_CC_END
