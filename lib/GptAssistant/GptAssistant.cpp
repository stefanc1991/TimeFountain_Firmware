#include "GptAssistant.h"

GptAssistant::GptAssistant(OpenAiApi* api, const AssistantInfo& assistantInfo)
    : _pOpenAiApi(api), _aiAssistantInfo(assistantInfo) {
    resetThread(); // Start with a fresh thread for this assistant
}

// ----- Getters -----

String GptAssistant::getId() const {
    return _aiAssistantInfo.id;
}

unsigned long GptAssistant::getCreatedAt() const {
    return _aiAssistantInfo.created_at;
}

String GptAssistant::getName() const {
    return _aiAssistantInfo.name;
}

String GptAssistant::getDescription() const {
    return _aiAssistantInfo.description;
}

String GptAssistant::getModel() const {
    return _aiAssistantInfo.model;
}

String GptAssistant::getInstructions() const {
    return _aiAssistantInfo.instructions;
}

std::vector<String> GptAssistant::getTools() const {
    return _aiAssistantInfo.tools;
}

JsonObject GptAssistant::getMetadata() const {
    return _aiAssistantInfo.metadata;
}

double GptAssistant::getTopP() const {
    return _aiAssistantInfo.top_p;
}

double GptAssistant::getTemperature() const {
    return _aiAssistantInfo.temperature;
}

String GptAssistant::getResponseFormat() const {
    return _aiAssistantInfo.response_format;
}

// ----- Setters -----

void GptAssistant::setModel(const String& model) {
    _aiAssistantInfo.model = model;
    _pOpenAiApi->modifyAssistant(getId(), "model", model);
}

void GptAssistant::setName(const String& name) {
    _aiAssistantInfo.name = name;
    _pOpenAiApi->modifyAssistant(getId(), "name", name);
}

void GptAssistant::setDescription(const String& description) {
    _aiAssistantInfo.description = description;
    _pOpenAiApi->modifyAssistant(getId(), "description", description);
}

void GptAssistant::setInstructions(const String& instructions) {
    _aiAssistantInfo.instructions = instructions;
    _pOpenAiApi->modifyAssistant(getId(), "instructions", instructions);
}

void GptAssistant::addTool(String toolString) {
    _aiAssistantInfo.tools.push_back(toolString);

    // Manually format tool list into JSON array string
    String toolsArrayString = "[";
    for (size_t i = 0; i < _aiAssistantInfo.tools.size(); ++i) {
        toolsArrayString += _aiAssistantInfo.tools[i];
        if (i < _aiAssistantInfo.tools.size() - 1) {
            toolsArrayString += ",";
        }
    }
    toolsArrayString += "]";

    _pOpenAiApi->modifyAssistant(getId(), "tools", toolsArrayString);
}

void GptAssistant::setToolResources(const JsonObject& toolResources) {
    _aiAssistantInfo.metadata = toolResources;
    String toolResourcesStr;
    serializeJson(toolResources, toolResourcesStr);
    _pOpenAiApi->modifyAssistant(getId(), "tool_resources", toolResourcesStr);
}

void GptAssistant::setTemperature(double temperature) {
    _aiAssistantInfo.temperature = temperature;
    _pOpenAiApi->modifyAssistant(getId(), "temperature", temperature);
}

void GptAssistant::setTopP(double top_p) {
    _aiAssistantInfo.top_p = top_p;
    _pOpenAiApi->modifyAssistant(getId(), "top_p", top_p);
}

void GptAssistant::setResponseFormat(const String& response_format) {
    _aiAssistantInfo.response_format = response_format;
    _pOpenAiApi->modifyAssistant(getId(), "response_format", response_format);
}

void GptAssistant::setToolCallback(const std::function<String(const String&, const String&)> callback) {
    _toolCallback = callback;
}

void GptAssistant::resetThread() {
    if (!_threadInfo.id.isEmpty()) {
        _pOpenAiApi->deleteThread(_threadInfo.id); // Clean up previous thread
    }
    _threadInfo = _pOpenAiApi->createThread();
}

String GptAssistant::sendMessage(const String& prompt) {

    MessageInfo messageInfo = _pOpenAiApi->createMessage(_threadInfo.id, "user", prompt);

    if (messageInfo.id.isEmpty()) {
        Serial.println("Error: Could not add message");
        return "Error: Could not add message";
    }

    RunInfo runInfo = _pOpenAiApi->createRun(_threadInfo.id, _aiAssistantInfo.id, "auto");

    // Poll run status until it's done or requires action
    while (runInfo.status == "queued" || runInfo.status == "in_progress" || runInfo.status == "cancelling") {
        vTaskDelay(500 / portTICK_PERIOD_MS);
        runInfo = _pOpenAiApi->retrieveRun(_threadInfo.id, runInfo.id);

        // Handle required tool call
        if (runInfo.status == "requires_action") {
            String toolCallId = runInfo.required_action.submit_tool_outputs.tool_calls[0].id;
            String toolFunctionName = runInfo.required_action.submit_tool_outputs.tool_calls[0].function.name;
            String toolFunctionParameters = runInfo.required_action.submit_tool_outputs.tool_calls[0].function.arguments;

            Serial.println(toolFunctionName);
            Serial.println(toolFunctionParameters);

            String toolCallOutput = "No callback function found. Contact user!";

            if (_toolCallback) {
                toolCallOutput = _toolCallback(toolFunctionName, toolFunctionParameters);
            }

            Serial.println(toolCallOutput);
            runInfo = _pOpenAiApi->submitToolOutput(runInfo.thread_id, runInfo.id, toolCallId, toolCallOutput);
        }
    }

    String result = "";

    if (runInfo.status == "completed") {
        std::vector<MessageInfo> messageList = _pOpenAiApi->listMessages(runInfo.thread_id, runInfo.id);

        for (const auto& message : messageList) {
            for (const auto& content : message.content) {
                if (content.type == "text") {
                    result += content.text.value + "\n";
                }
            }
            result += "\n";
        }
    } else {
        Serial.print("Run did not complete. Status: ");
        Serial.println(runInfo.status);
        result = "Run did not complete. Status: " + runInfo.status;
    }

    return result;
}
