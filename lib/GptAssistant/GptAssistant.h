#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include <functional>
#include "OpenAiApi.h"
#include "InfoStructs.h"

class OpenAiApi;

class GptAssistant {
public:
    GptAssistant(OpenAiApi* api, const AssistantInfo& assistantData);

    // Getters
    String getId() const;
    unsigned long getCreatedAt() const;
    String getName() const;
    String getDescription() const;
    String getModel() const;
    String getInstructions() const;
    std::vector<String> getTools() const;
    JsonObject getMetadata() const;
    double getTopP() const;
    double getTemperature() const;
    String getResponseFormat() const;

    // Setters
    void setModel(const String& model);
    void setName(const String& name);
    void setDescription(const String& description);
    void setInstructions(const String& instructions);
    void addTool(String toolJson);
    void setToolResources(const JsonObject& toolResources);
    void setTemperature(double temperature);
    void setTopP(double top_p);
    void setResponseFormat(const String& response_format);

    void setToolCallback(const std::function<String(const String&, const String&)> callback);
    void resetThread();
    String sendMessage(const String& prompt);

private:
    OpenAiApi* _pOpenAiApi;
    AssistantInfo _aiAssistantInfo;
    ThreadInfo _threadInfo;
    std::function<String(const String&, const String&)> _toolCallback;
};
