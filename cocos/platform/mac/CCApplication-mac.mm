/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2013-2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

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
#include "platform/CCApplication.h"
#import <Cocoa/Cocoa.h>
#include <algorithm>
#include <mutex>
#include "base/CCScheduler.h"
#include "base/CCAutoreleasePool.h"
#include "base/CCGLUtils.h"
#include "base/CCConfiguration.h"
#include "platform/desktop/CCGLView-desktop.h"
#include "scripting/js-bindings/event/EventDispatcher.h"
#include "scripting/js-bindings/jswrapper/SeApi.h"
#include "base/CCGLUtils.h"
#include "audio/include/AudioEngine.h"

NS_CC_BEGIN

namespace
{
    int g_width = 0;
    int g_height = 0;
    bool setCanvasCallback(se::Object* global)
    {
        se::ScriptEngine* se = se::ScriptEngine::getInstance();
        char commandBuf[200] = {0};
        int devicePixelRatio = Application::getInstance()->getDevicePixelRatio();
        sprintf(commandBuf, "window.innerWidth = %d; window.innerHeight = %d;",
                g_width / devicePixelRatio,
                g_height / devicePixelRatio);
        se->evalString(commandBuf);
        ccViewport(0, 0, g_width / devicePixelRatio, g_height / devicePixelRatio);
        glDepthMask(GL_TRUE);
        return true;
    }
}

std::shared_ptr<ApplicationImpl> ApplicationImpl::_instance = nullptr;

#define CAST_VIEW(view)    ((GLView*)view)

ApplicationImpl::ApplicationImpl(const std::string& name, int width, int height)
{   
    g_width = width;
    g_height = height;
    
    createView(name, width, height);

    Configuration::getInstance();

    _renderTexture = new RenderTexture(width, height);
    _scheduler = new Scheduler();
    
    EventDispatcher::init();
    se::ScriptEngine::getInstance();
}

ApplicationImpl::~ApplicationImpl()
{
    EventDispatcher::destroy();
    se::ScriptEngine::destroyInstance();

    // close audio device
    cocos2d::experimental::AudioEngine::end();
    
    delete CAST_VIEW(_view);
    _view = nullptr;
    
    delete _scheduler;
    _scheduler = nullptr;
    
    delete _renderTexture;
    _renderTexture = nullptr;
}

void ApplicationImpl::start()
{
    if (!_view)
        return;

    float dt = 0.f;
    long long actualInternal = 0; // actual frame internal
    long long desiredInterval = 0; // desired frame internal, 1 / fps

    std::chrono::steady_clock::time_point prev;
    std::chrono::steady_clock::time_point now;

    prev = std::chrono::steady_clock::now();

    se::ScriptEngine* se = se::ScriptEngine::getInstance();

    while (!CAST_VIEW(_view)->windowShouldClose())
    {
        desiredInterval = 1.0 / _fps * 1000000;
        if (!_isStarted)
        {
            auto scheduler = Application::getInstance()->getScheduler();
            scheduler->removeAllFunctionsToBePerformedInCocosThread();
            scheduler->unscheduleAll();

            se::ScriptEngine::getInstance()->cleanup();
            cocos2d::PoolManager::getInstance()->getCurrentPool()->clear();
            cocos2d::EventDispatcher::init();

            ccInvalidateStateCache();
            se->addRegisterCallback(setCanvasCallback);

            if(!applicationDidFinishLaunching())
                return;

            _isStarted = true;
        }

        // should be invoked at the begin of rendering a frame
        if (_isDownsampleEnabled)
            _renderTexture->prepare();

        CAST_VIEW(_view)->pollEvents();

        if (_isStarted)
        {
            now = std::chrono::steady_clock::now();
            actualInternal = std::chrono::duration_cast<std::chrono::microseconds>(now - prev).count();
            if (actualInternal >= desiredInterval)
            {
                prev = now;
                dt = (float)actualInternal / 1000000.f;
                _scheduler->update(dt);

                EventDispatcher::dispatchTickEvent(dt);

                if (_isDownsampleEnabled)
                    _renderTexture->draw();

                CAST_VIEW(_view)->swapBuffers();
                PoolManager::getInstance()->getCurrentPool()->clear();
            }
            else
            {
                // sleep 3ms may make a sleep of 4ms
                std::this_thread::sleep_for(std::chrono::microseconds(desiredInterval - actualInternal - 1000));
            }
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::microseconds(desiredInterval));
        }
    }
}

void ApplicationImpl::restart()
{
    _isStarted = false;
}

void ApplicationImpl::end()
{
    glfwSetWindowShouldClose(CAST_VIEW(_view)->getGLFWWindow(), 1);
}

void ApplicationImpl::setPreferredFramesPerSecond(int fps)
{
    _fps = fps;
}

ApplicationImpl::Platform ApplicationImpl::getPlatform() const
{
    return Platform::MAC;
}

std::string ApplicationImpl::getCurrentLanguageCode() const
{
    static char code[3]={0};
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    NSArray *languages = [defaults objectForKey:@"AppleLanguages"];
    NSString *currentLanguage = [languages objectAtIndex:0];
    
    // get the current language code.(such as English is "en", Chinese is "zh" and so on)
    NSDictionary* temp = [NSLocale componentsFromLocaleIdentifier:currentLanguage];
    NSString * languageCode = [temp objectForKey:NSLocaleLanguageCode];
    [languageCode getCString:code maxLength:3 encoding:NSASCIIStringEncoding];
    code[2]='\0';
    return code;
}

bool ApplicationImpl::isDisplayStats() {
    se::AutoHandleScope hs;
    se::Value ret;
    char commandBuf[100] = "cc.debug.isDisplayStats();";
    se::ScriptEngine::getInstance()->evalString(commandBuf, 100, &ret);
    return ret.toBoolean();
}

void ApplicationImpl::setDisplayStats(bool isShow) {
    se::AutoHandleScope hs;
    char commandBuf[100] = {0};
    sprintf(commandBuf, "cc.debug.setDisplayStats(%s);", isShow ? "true" : "false");
    se::ScriptEngine::getInstance()->evalString(commandBuf);
}

ApplicationImpl::LanguageType ApplicationImpl::getCurrentLanguage() const
{
    // get the current language and country config
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    NSArray *languages = [defaults objectForKey:@"AppleLanguages"];
    NSString *currentLanguage = [languages objectAtIndex:0];
    
    // get the current language code.(such as English is "en", Chinese is "zh" and so on)
    NSDictionary* temp = [NSLocale componentsFromLocaleIdentifier:currentLanguage];
    NSString * languageCode = [temp objectForKey:NSLocaleLanguageCode];
    
    if ([languageCode isEqualToString:@"zh"]) return LanguageType::CHINESE;
    if ([languageCode isEqualToString:@"en"]) return LanguageType::ENGLISH;
    if ([languageCode isEqualToString:@"fr"]) return LanguageType::FRENCH;
    if ([languageCode isEqualToString:@"it"]) return LanguageType::ITALIAN;
    if ([languageCode isEqualToString:@"de"]) return LanguageType::GERMAN;
    if ([languageCode isEqualToString:@"es"]) return LanguageType::SPANISH;
    if ([languageCode isEqualToString:@"nl"]) return LanguageType::DUTCH;
    if ([languageCode isEqualToString:@"ru"]) return LanguageType::RUSSIAN;
    if ([languageCode isEqualToString:@"ko"]) return LanguageType::KOREAN;
    if ([languageCode isEqualToString:@"ja"]) return LanguageType::JAPANESE;
    if ([languageCode isEqualToString:@"hu"]) return LanguageType::HUNGARIAN;
    if ([languageCode isEqualToString:@"pt"]) return LanguageType::PORTUGUESE;
    if ([languageCode isEqualToString:@"ar"]) return LanguageType::ARABIC;
    if ([languageCode isEqualToString:@"nb"]) return LanguageType::NORWEGIAN;
    if ([languageCode isEqualToString:@"pl"]) return LanguageType::POLISH;
    if ([languageCode isEqualToString:@"tr"]) return LanguageType::TURKISH;
    if ([languageCode isEqualToString:@"uk"]) return LanguageType::UKRAINIAN;
    if ([languageCode isEqualToString:@"ro"]) return LanguageType::ROMANIAN;
    if ([languageCode isEqualToString:@"bg"]) return LanguageType::BULGARIAN;
    return LanguageType::ENGLISH;
}

float ApplicationImpl::getScreenScale() const
{
    return CAST_VIEW(_view)->getScale();
}

GLint ApplicationImpl::getMainFBO() const
{
    return CAST_VIEW(_view)->getMainFBO();
}

bool ApplicationImpl::openURL(const std::string &url)
{
    NSString* msg = [NSString stringWithCString:url.c_str() encoding:NSUTF8StringEncoding];
    NSURL* nsUrl = [NSURL URLWithString:msg];
    return [[NSWorkspace sharedWorkspace] openURL:nsUrl];
}

bool ApplicationImpl::applicationDidFinishLaunching()
{
    return true;
}

void ApplicationImpl::applicationDidEnterBackground()
{
}

void ApplicationImpl::applicationWillEnterForeground()
{
}

void ApplicationImpl::setMultitouch(bool)
{
}

void Application::onCreateView(PixelFormat& pixelformat, DepthFormat& depthFormat, int& multisamplingCount)
{
    pixelformat = PixelFormat::RGBA8;
    depthFormat = DepthFormat::DEPTH24_STENCIL8;

    multisamplingCount = 0;
}

void ApplicationImpl::createView(const std::string& name, int width, int height)
{
    int multisamplingCount = 0;
    PixelFormat pixelformat;
    DepthFormat depthFormat;
    
    onCreateView(pixelformat,
                 depthFormat,
                 multisamplingCount);

    _view = new GLView(this, name, 0, 0, width, height, pixelformat, depthFormat, multisamplingCount);
}

std::string ApplicationImpl::getSystemVersion()
{
    NSOperatingSystemVersion v = NSProcessInfo.processInfo.operatingSystemVersion;
    char version[50] = {0};
    snprintf(version, sizeof(version), "%d.%d.%d", (int)v.majorVersion, (int)v.minorVersion, (int)v.patchVersion);
    return version;
}

NS_CC_END
