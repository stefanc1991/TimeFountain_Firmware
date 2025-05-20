#pragma once

#include <Arduino.h>
#include <FastLED.h>
#include <vector>
#include "PatternInfoStructs.h"

class WaterStream {
    
public:
    WaterStream(uint16_t tUsDutyCycle, uint16_t initialShift);
    
    void init();
    void setPeriod(uint16_t tUsPeriod);
    void setNextStateChangeDelay(uint16_t tUsDelay);
    void moveOn();
    bool isActive();
    unsigned long getNextStateChangeTime();
    CRGB getTopColor();
    CRGB getBotColor();
    void updateMotion(const unsigned long tMsNow);
    void updateVisibility(const unsigned long tMsNow);
    void updateColorTop(const unsigned long tMsNow);
    void updateColorBot(const unsigned long tMsNow);

    static WaterStream fromStruct(const WaterStreamInfo& info);

private:

    uint16_t _tUsDutyCycle;
    uint16_t _tUsPeriod;
    uint16_t _initialShift;
    
    unsigned long _tUsNextRegularStateChange;
    uint16_t _tUsNextStateChangeDelay;
    enum StreamState { INACTIVE, ACTIVE } _state;

    int16_t _tUsCurrentVelocity;
    uint8_t _currentVisibility;
    CRGB _currentTopColor;
    CRGB _currentBotColor;

    std::vector<ColorPhaseInfo> _topColorPhases;
    std::vector<ColorPhaseInfo> _botColorPhases;
    std::vector<VisibilityPhaseInfo> _visibilityPhases;
    std::vector<MotionPhaseInfo> _motionPhases;

    size_t _iThisMotionPhase;
    size_t _iThisTopColorPhase;
    size_t _iThisBotColorPhase;
    size_t _iThisVisibilityPhase;
    
    unsigned long _tMsLastMotionPhaseChange;
    unsigned long _tMsLastVisibilityPhaseChange;
    unsigned long _tMsLastTopColorPhaseChange;
    unsigned long _tMsLastBotColorPhaseChange;

    CRGB _scaleColor(CRGB color, uint8_t scale);
};