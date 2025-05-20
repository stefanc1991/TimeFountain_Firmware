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

#include "pins.h"
#include "definitions.h"

extern StateManager stateManager;
extern hw_timer_t* pTimerLight;

extern AsyncWebServer server;

extern void flashLeds(struct CRGB* leds);

void onWiFiConnected(WiFiEvent_t event, WiFiEventInfo_t info);
void onWiFiDisconnected(WiFiEvent_t event, WiFiEventInfo_t info);
void onApConnected(WiFiEvent_t event, WiFiEventInfo_t info);
void onUploadStart();
void onUploadProgress(size_t current, size_t final);
void onUploadEnd(bool success);
