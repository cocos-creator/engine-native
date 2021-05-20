/****************************************************************************
 Copyright (c) 2018-2021 Xiamen Yaji Software Co., Ltd.

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

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace se {
class Value;
}

namespace cc {

// Touch event related

struct TouchInfo {
    float x     = 0;
    float y     = 0;
    int   index = 0;

    TouchInfo(float x, float y, int index)
    : x(x),
      y(y),
      index(index) {}
};

struct TouchEvent {
    enum class Type {
        BEGAN,
        MOVED,
        ENDED,
        CANCELLED,
        UNKNOWN
    };

    std::vector<TouchInfo> touches;
    Type                   type = Type::UNKNOWN;
};

struct MouseEvent {
    enum class Type {
        DOWN,
        UP,
        MOVE,
        WHEEL,
        UNKNOWN
    };

    float x = 0.0F;
    float y = 0.0F;
    // The button number that was pressed when the mouse event was fired: Left button=0, middle button=1 (if present), right button=2.
    // For mice configured for left handed use in which the button actions are reversed the values are instead read from right to left.
    uint16_t button = 0;
    Type     type   = Type::UNKNOWN;
};

enum class KeyCode {
    BACKSPACE       = 8,
    TAB             = 9,
    NUM_LOCK        = 12,
    NUMPAD_ENTER    = 20013,
    ENTER           = 13,
    SHIFT_RIGHT     = 20016,
    SHIFT_LEFT      = 16,
    CONTROL_LEFT    = 17,
    CONTROL_RIGHT   = 20017,
    ALT_RIGHT       = 20018,
    ALT_LEFT        = 18,
    CAPS_LOCK       = 20,
    ESCAPE          = 27,
    SPACE           = 32,
    PAGE_UP         = 33,
    PAGE_DOWN       = 34,
    END             = 35,
    HOME            = 36,
    ARROW_LEFT      = 37,
    ARROW_UP        = 38,
    ARROW_RIGHT     = 39,
    ARROW_DOWN      = 40,
    Delete          = 46, // NOLINT
    META_LEFT       = 91,
    CONTEXT_MENU    = 20093,
    META_RIGHT      = 93,
    NUMPAD_MULTIPLY = 106,
    NUMPAD_PLUS     = 107,
    NUMPAD_MINUS    = 109,
    NUMPAD_DECIMAL  = 110,
    NUMPAD_DIVIDE   = 111,
    SEMICOLON       = 186,
    EQUAL           = 187,
    COMMA           = 188,
    MINUS           = 189,
    PERIOD          = 190,
    SLASH           = 191,
    BACKQUOTE       = 192,
    BRACKET_LEFT    = 219,
    BACKSLASH       = 220,
    BRACKET_RIGHT   = 221,
    QUOTE           = 222,
    NUMPAD_0        = 10048,
    NUMPAD_1        = 10049,
    NUMPAD_2        = 10050,
    NUMPAD_3        = 10051,
    NUMPAD_4        = 10052,
    NUMPAD_5        = 10053,
    NUMPAD_6        = 10054,
    NUMPAD_7        = 10055,
    NUMPAD_8        = 10056,
    NUMPAD_9        = 10057
};

struct KeyboardEvent {
    enum class Action {
        PRESS,
        RELEASE,
        REPEAT,
        UNKNOWN
    };

    int    key            = -1;
    Action action         = Action::UNKNOWN;
    bool   altKeyActive   = false;
    bool   ctrlKeyActive  = false;
    bool   metaKeyActive  = false;
    bool   shiftKeyActive = false;
    // TODO(mingo): support caps lock?
};

class CustomEvent {
public:
    std::string name;
    union {
        void *  ptrVal;
        int32_t longVal;
        int     intVal;
        int16_t shortVal;
        char    charVal;
        bool    boolVal;
    } args[10];

    CustomEvent()          = default;
    virtual ~CustomEvent() = default;
};

class EventDispatcher {
public:
    static void init();
    static void destroy();
    static bool initialized();

    static void dispatchTouchEvent(const struct TouchEvent &touchEvent);
    static void dispatchMouseEvent(const struct MouseEvent &mouseEvent);
    static void dispatchKeyboardEvent(const struct KeyboardEvent &keyboardEvent);
    static void dispatchTickEvent(float dt);
    static void dispatchResizeEvent(int width, int height);
    static void dispatchOrientationChangeEvent(int orientation);
    static void dispatchEnterBackgroundEvent();
    static void dispatchEnterForegroundEvent();
    static void dispatchMemoryWarningEvent();
    static void dispatchRestartVM();
    static void dispatchCloseEvent();

    using CustomEventListener = std::function<void(const CustomEvent &)>;
    static uint32_t addCustomEventListener(const std::string &eventName, const CustomEventListener &listener);
    static void     removeCustomEventListener(const std::string &eventName, uint32_t listenerID);
    static void     removeAllCustomEventListeners(const std::string &eventName);
    static void     removeAllEventListeners();
    static void     dispatchCustomEvent(const CustomEvent &event);

private:
    static void doDispatchEvent(const char *eventName, const char *jsFunctionName, const std::vector<se::Value> &args);

    struct Node {
        CustomEventListener listener;
        uint32_t            listenerID;
        struct Node *       next = nullptr;
    };
    static std::unordered_map<std::string, Node *> listeners;
    static uint32_t                                hashListenerId; //simple increment hash
};

} // end of namespace cc
