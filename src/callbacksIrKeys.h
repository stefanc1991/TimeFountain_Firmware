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
#include "Pattern.h"
#include "Pump.h"
#include "ElectroMagnet.h"
#include "PatternManager.h"

#include "pins.h"
#include "definitions.h"

extern State* pStatePlay;
extern State* pStateStandby;
extern State* pStateConfig;

extern uint16_t vibrationPeriod;
extern ElectroMagnet elMagnet;

extern PatternManager patternManagerPlay;
extern PatternManager patternManagerConfig;

extern Pattern* pActivePattern;

extern QueueHandle_t queueLightEvents;

extern hw_timer_t *pTimerMagnet;
extern hw_timer_t* pTimerLight;

extern void restartLights();

void onClickIrKeyPlay();
void onClickIrMenu();
void onClickIrKeyUp();
void onClickIrKeyDown();
void onClickIrKeyRight();
void onClickIrKeyLeft();
void onClickIrKeyOk();
