#pragma once

#include <memory>
#include <vector>

#include <iosfwd>
#include <cstdint>

#include "math/Vec2.h"
#include "math/CCGeometry.h"
#include "base/ccTypes.h"

#include "base/ccConfig.h"
#if CC_ENABLE_TTF_LABEL_RENDERER

namespace cocos2d {


    // ref CCMacro.js
    enum class LabelAlignmentH : uint8_t
    {
        LEFT, CENTER, RIGHT
    };
    // ref CCMacro.js
    enum class LabelAlignmentV : uint8_t {
        TOP, CENTER, BOTTOM
    };
    // ref CCLabel.js
    enum class LabelOverflow : uint8_t {
        NONE, CLAMP, SHRINK, RESIZE_HEIGHT
    };

    struct LabelLayoutInfo {
        float lineHeight = 0.0f;
        float outlineSize = 0.0f;
        float spaceX = 0.0f;
        float width = 0.0f;
        float height = 0.0f; 
        float anchorX = 0.5f;
        float anchorY = 0.5f;
        float shadowX = 0.0f;
        float shadowY = 0.0f;
        int32_t shadowBlur = -1;
        Color4B shadowColor;
        Color4B color;
        Color4B outlineColor;
        bool wrap = false;
        bool bold = false;
        bool italic = false;
        bool underline = false;
        LabelAlignmentV valign = LabelAlignmentV::CENTER;
        LabelAlignmentH halign = LabelAlignmentH::CENTER;
        LabelOverflow overflow = LabelOverflow::NONE;
    };

    enum class PixelMode {
        AI88,
        A8,
        RGB888,
        BGRA8888,
        INVAL,
    };

    int PixelModeSize(PixelMode mode);

    class GlyphBitmap {
    public:
        GlyphBitmap(std::vector<uint8_t>& data, int width, int height, Rect rect, int xAdvance, PixelMode mode, int outline);

        GlyphBitmap(GlyphBitmap&& other) noexcept;

        int getWidth() const { return _width; }
        int getHeight() const { return _height; }
        Rect getRect() const { return _rect; }
        int getXAdvance() const { return _xAdvance; }
        int getOutline() const { return _outline; }
        PixelMode getPixelMode() const { return _pixelMode; }
        std::vector<uint8_t>& getData() { return _data; }

    private:
        int _width = 0;
        int _height = 0;
        int _outline = 0;
        std::vector<uint8_t> _data;
        Rect _rect;
        int _xAdvance = 0;
        PixelMode _pixelMode;
    };
}

#endif
