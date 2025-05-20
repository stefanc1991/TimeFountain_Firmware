#pragma once
#define ARDUINOJSON_DEFAULT_NESTING_LIMIT 20
#include <Arduino.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "GptAssistant.h"
#include "InfoStructs.h"

#include <variant>


class GptAssistant;

class OpenAiApi {
public:
    OpenAiApi(const String& apiKey, const String& serverName);

    bool createAssistant(const String& model, const String& name, GptAssistant*& pAssistant);

    bool retrieveAssistant(const String& assistant_id, GptAssistant*& pAssistant);

    std::vector<AssistantInfo> listAssistants(uint8_t limit = 0);

    bool searchAssistant(const String& name, GptAssistant*& pAssistant);

    void modifyAssistant(const String& assistant_id, const String& parameter, const String& value);
    void modifyAssistant(const String& assistant_id, const String& parameter, double value);
    
    ThreadInfo createThread();
    bool deleteThread(const String& thread_id);

    MessageInfo createMessage(const String& thread_id, const String& role, const String& content);
    std::vector<MessageInfo> listMessages(const String& thread_id, const String& run_id);

    RunInfo createRun(const String& thread_id, const String& assistant_id, const String& tool_choice = "auto");
    RunInfo retrieveRun(const String& thread_id, const String& run_id);
    RunInfo submitToolOutput(const String& thread_id, const String& run_id, const String& tool_call_id, const String& output);

private:
    String _apiKey;
    String _serverName;

    String _sendHttpRequest(const String& url, const String& method, const String& payload = "");
};
