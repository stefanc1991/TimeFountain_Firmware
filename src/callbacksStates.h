#pragma once

#include <Arduino.h>

#include <WiFi.h>
#include <WiFiClient.h>
#include <ESPAsyncWebServer.h>
#include <FastLED.h>

#include "LittleFsHelper.h"
#include "IrManager.h"
#include "SettingsManager.h"
#include "StateManager.h"
#include "WifiManager.h"
#include "WebPageSetup.h"
#include "ClampedInteger.h"
#include "CyclicInteger.h"
#include "Pump.h"
#include "ElectroMagnet.h"
#include "PatternManager.h"

#include "pins.h"
#include "definitions.h"

extern Pump pump;
extern ElectroMagnet elMagnet;

extern State* pStateConfig;
extern State* pStatePlay;
extern State* pStateStandby;

extern PatternManager patternManagerPlay;
extern PatternManager patternManagerConfig;

extern Pattern* pActivePattern;

extern hw_timer_t* pTimerLight;

extern void restartLights();
extern void flashLeds(struct CRGB* leds);

void onStartStatePlay(State * pLastState);
void onLoopStatePlay();
void onEndStatePlay(State* pNextState);

void onStartStateConfig(State* pLastState);
void onLoopStateConfig();
void onEndStateConfig(State* pNextState);

void onLoopStateStandby();