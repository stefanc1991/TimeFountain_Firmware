#pragma once

#include <Arduino.h>

#include <WiFi.h>
#include <WiFiClient.h>
#include <ESPAsyncWebServer.h>
#include <FastLED.h>

#include "LittleFsHelper.h"
#include "IrManager.h"
#include "StateManager.h"
#include "WifiManager.h"
#include "WebPageSetup.h"
#include "ClampedInteger.h"
#include "CyclicInteger.h"
#include "Pattern.h"

#include "pins.h"
#include "definitions.h"

extern TaskHandle_t magnetControlTaskHandler;
extern TaskHandle_t lightControlTaskHandler;

void IRAM_ATTR timerInterruptLightEvent();
void IRAM_ATTR timerInterruptMagnet();
