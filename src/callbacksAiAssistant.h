#pragma once

#include <Arduino.h>

#include <ArduinoJson.h>

#include "Pattern.h"
#include "PatternManager.h"
#include "GptAssistant.h"
#include "OpenAiApi.h"
#include "WebPageAiChat.h"
#include "ElectroMagnet.h"
#include "OpenAiApi.h"

#include "definitions.h"
#include "assistant_definitions.h"

extern Pattern* pActivePattern;

extern PatternManager patternManagerPlay;
extern PatternManager patternManagerConfig;

extern ElectroMagnet elMagnet;
extern GptAssistant* pAssistant;

extern OpenAiApi openAiApi;

extern void restartLights();

String toolCallback(const String &toolName, const String &parameters);
bool undoLastChangesCallback();
void saveChangesCallback();
void aiChatMessageCallback(String message);
