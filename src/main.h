
#include <Arduino.h>

#include <WiFi.h>
#include <WiFiClient.h>
#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>
#include <Update.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <FastLED.h>

#include "Pattern.h"
#include "WaterStream.h"
#include "Pump.h"
#include "ElectroMagnet.h"
#include "LittleFsHelper.h"
#include "WifiManager.h"
#include "PatternManager.h"
#include "CyclicInteger.h"
#include "ClampedInteger.h"
#include "IrManager.h"
#include "SettingsManager.h"
#include "GptAssistant.h"
#include "OpenAiApi.h"
#include "WebPageSetup.h"
#include "WebPageAiChat.h"
#include "WebPageHome.h"

#include "pins.h"
#include "definitions.h"
#include "enumsAndStructs.h"
#include "callbacksIrKeys.h"
#include "callbacksStates.h"
#include "callbacksOther.h"
#include "functionsOther.h"
#include "interrupts.h"
#include "callbacksAiAssistant.h"
#include "definition_openai_key.h"

// Global variable declarations
size_t totalHeap;  // for debugging

AsyncWebServer server(80);

GptAssistant* pAssistant;
OpenAiApi openAiApi(OPEN_AI_API_KEY, SERVER_NAME);

StateManager stateManager(INDEPENDENT);

State* pStateConfig;
State* pStatePlay;
State* pStateStandby;

Pump pump(PUMP_CTRL_PIN, PUMP_GND_PIN, PUMP_PWM_PIN, DEFAULT_PUMP_POWER);
ElectroMagnet elMagnet(MAGNET_CTRL_PIN_1, MAGNET_CTRL_PIN_2, MAGNET_PWM_PIN);

Pattern* pActivePattern;
uint16_t vibrationPeriod;

PatternManager patternManagerPlay(MAX_BACKUPS);
PatternManager patternManagerConfig(0);

bool lightsOn = false;
bool resetSchedularFlag = false;

hw_timer_t* pTimerMagnet = nullptr;
hw_timer_t* pTimerLight = nullptr;

TaskHandle_t eventSchedularTaskHandler;
TaskHandle_t lightControlTaskHandler;
TaskHandle_t magnetControlTaskHandler;

QueueHandle_t queueLightEvents;

portMUX_TYPE DRAM_ATTR timerMux = portMUX_INITIALIZER_UNLOCKED;

// Functions Declaration
void magnetControlTask(void* pvParameters);
void lightControlTask(void* pvParameters);
void lightEventSchedularTask(void* pvParameters);
void restartLights();
