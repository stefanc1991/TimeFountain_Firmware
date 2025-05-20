#include "OpenAiApi.h"

OpenAiApi::OpenAiApi(const String& apiKey, const String& serverName)
    : _apiKey(apiKey), _serverName(serverName) {}

// Creates a new assistant and assigns to pAssistant
bool OpenAiApi::createAssistant(const String& model, const String& name, GptAssistant*& pAssistant) {
    delete pAssistant;
    pAssistant = nullptr;

    JsonDocument doc;
    doc["model"] = model;
    doc["name"] = name;

    String payload;
    serializeJson(doc, payload);

    String httpResponse = _sendHttpRequest(_serverName + "/assistants", "POST", payload);

    if (!httpResponse.isEmpty()) {
        JsonDocument responseDoc;
        DeserializationError error = deserializeJson(responseDoc, httpResponse);
        if (!error) {
            JsonObject assistantData = responseDoc.as<JsonObject>();
            AssistantInfo info = AssistantInfo::fromJson(assistantData);
            pAssistant = new GptAssistant(this, info);
            return true;
        } else {
            Serial.print("JSON-Deserialisierung fehlgeschlagen: ");
            Serial.println(error.c_str());
        }
    }

    return false;
}

// Loads assistant by ID and assigns to pAssistant
bool OpenAiApi::retrieveAssistant(const String& assistant_id, GptAssistant*& pAssistant) {
    delete pAssistant;
    pAssistant = nullptr;

    String httpResponse = _sendHttpRequest(_serverName + "/assistants/" + assistant_id, "GET");

    if (!httpResponse.isEmpty()) {
        JsonDocument responseDoc;
        DeserializationError error = deserializeJson(responseDoc, httpResponse);
        if (!error) {
            JsonObject assistantData = responseDoc.as<JsonObject>();
            AssistantInfo info = AssistantInfo::fromJson(assistantData);
            pAssistant = new GptAssistant(this, info);
            return true;
        } else {
            Serial.print("JSON-Deserialisierung fehlgeschlagen: ");
            Serial.println(error.c_str());
        }
    }

    return false;
}

// Lists assistant definitions, optionally limited
std::vector<AssistantInfo> OpenAiApi::listAssistants(uint8_t limit) {
    String url = _serverName + "/assistants";
    if (limit > 0) {
        url += "?limit=" + String(limit);
    }

    String httpResponse = _sendHttpRequest(url, "GET");
    std::vector<AssistantInfo> assistantsList;

    if (!httpResponse.isEmpty()) {
        JsonDocument responseDoc;
        DeserializationError error = deserializeJson(responseDoc, httpResponse);
        if (!error) {
            JsonArray assistantsArray = responseDoc["data"].as<JsonArray>();
            for (JsonObject assistantJson : assistantsArray) {
                AssistantInfo info = AssistantInfo::fromJson(assistantJson);
                assistantsList.push_back(info);
                break; // Only get first if limit == 1
            }
        } else {
            Serial.print("JSON-Deserialisierung fehlgeschlagen: ");
            Serial.println(error.c_str());
        }
    }

    return assistantsList;
}

// Finds an assistant by name
bool OpenAiApi::searchAssistant(const String& name, GptAssistant*& pAssistant) {
    delete pAssistant;
    pAssistant = nullptr;

    std::vector<AssistantInfo> assistantsList = listAssistants(1);

    for (const AssistantInfo& info : assistantsList) {
        if (info.name == name) {
            pAssistant = new GptAssistant(this, info);
            return true;
        }
    }

    return false;
}

// Modify assistant parameter (string version)
void OpenAiApi::modifyAssistant(const String& assistant_id, const String& parameter, const String& value) {
    JsonDocument doc;

    // Try to parse value if it's a JSON array
    if (value.startsWith("[")) {
        DeserializationError error = deserializeJson(doc[parameter], value);
        if (error) {
            Serial.print("Fehler beim Deserialisieren des JSON-Werts: ");
            Serial.println(error.c_str());
            Serial.println(value);
            delay(10000);
            return;
        }
    } else {
        doc[parameter] = value;
    }

    String payload;
    serializeJson(doc, payload);

    _sendHttpRequest(_serverName + "/assistants/" + assistant_id, "POST", payload);
}

// Modify assistant parameter (numeric version)
void OpenAiApi::modifyAssistant(const String& assistant_id, const String& parameter, double value) {
    JsonDocument doc;
    doc[parameter] = value;

    String payload;
    serializeJson(doc, payload);

    _sendHttpRequest(_serverName + "/assistants/" + assistant_id, "POST", payload);
}

// Creates a new assistant thread
ThreadInfo OpenAiApi::createThread() {
    String url = _serverName + "/threads";
    String httpResponse = _sendHttpRequest(url, "POST", "");

    JsonDocument responseDoc;
    if (!httpResponse.isEmpty()) {
        DeserializationError error = deserializeJson(responseDoc, httpResponse);
        if (!error) {
            return ThreadInfo::fromJson(responseDoc.as<JsonObject>());
        } else {
            Serial.print("JSON-Deserialisierung fehlgeschlagen: ");
            Serial.println(error.c_str());
        }
    }

    return ThreadInfo();
}

// Deletes a thread
bool OpenAiApi::deleteThread(const String& thread_id) {
    String url = _serverName + "/threads/" + thread_id;
    String httpResponse = _sendHttpRequest(url, "DELETE", "");

    JsonDocument responseDoc;
    if (!httpResponse.isEmpty()) {
        DeserializationError error = deserializeJson(responseDoc, httpResponse);
        if (!error) {
            return responseDoc["deleted"];
        } else {
            Serial.print("Failed to parse JSON response: ");
            Serial.println(error.c_str());
        }
    } else {
        Serial.println("No response received");
    }

    return false;
}

// Adds a user message to a thread
MessageInfo OpenAiApi::createMessage(const String& thread_id, const String& role, const String& content) {
    String url = _serverName + "/threads/" + thread_id + "/messages";

    JsonDocument doc;
    doc["role"] = role;
    doc["content"] = content;

    String payload;
    serializeJson(doc, payload);
    String httpResponse = _sendHttpRequest(url, "POST", payload);

    JsonDocument responseDoc;
    if (!httpResponse.isEmpty()) {
        DeserializationError error = deserializeJson(responseDoc, httpResponse);
        if (!error) {
            return MessageInfo::fromJson(responseDoc.as<JsonObject>());
        } else {
            Serial.print("JSON-Deserialisierung fehlgeschlagen: ");
            Serial.println(error.c_str());
        }
    }

    return MessageInfo();
}

// Fetches all messages in a thread (optionally filtered by run_id)
std::vector<MessageInfo> OpenAiApi::listMessages(const String& thread_id, const String& run_id) {
    String url = _serverName + "/threads/" + thread_id + "/messages";
    if (!run_id.isEmpty()) {
        url += "?run_id=" + run_id;
    }

    String httpResponse = _sendHttpRequest(url, "GET");
    std::vector<MessageInfo> messagesList;

    if (!httpResponse.isEmpty()) {
        JsonDocument responseDoc;
        DeserializationError error = deserializeJson(responseDoc, httpResponse);
        if (!error) {
            JsonArray messagesArray = responseDoc["data"].as<JsonArray>();
            for (JsonObject messageJson : messagesArray) {
                messagesList.push_back(MessageInfo::fromJson(messageJson));
            }
        } else {
            Serial.print("JSON deserialization failed: ");
            Serial.println(error.c_str());
        }
    }

    return messagesList;
}

// Submits output for a tool call within a run
RunInfo OpenAiApi::submitToolOutput(const String& thread_id, const String& run_id, const String& tool_call_id, const String& output) {
    String url = _serverName + "/threads/" + thread_id + "/runs/" + run_id + "/submit_tool_outputs";

    JsonDocument doc;
    JsonArray toolOutputsArray = doc["tool_outputs"].to<JsonArray>();
    JsonObject toolOutput = toolOutputsArray.add<JsonObject>();
    toolOutput["tool_call_id"] = tool_call_id;
    toolOutput["output"] = output;

    String payload;
    serializeJson(doc, payload);
    String response = _sendHttpRequest(url, "POST", payload);

    JsonDocument responseDoc;
    if (!response.isEmpty()) {
        DeserializationError error = deserializeJson(responseDoc, response);
        if (!error) {
            return RunInfo::fromJson(responseDoc.as<JsonObject>());
        } else {
            Serial.print("JSON-Deserialisierung fehlgeschlagen: ");
            Serial.println(error.c_str());
        }
    } else {
        Serial.println("Empty or failed response.");
    }

    return RunInfo();
}

// Starts a new run (assistant processing session)
RunInfo OpenAiApi::createRun(const String& thread_id, const String& assistant_id, const String& tool_choice) {
    String url = _serverName + "/threads/" + thread_id + "/runs";

    JsonDocument doc;
    doc["assistant_id"] = assistant_id;
    doc["tool_choice"] = tool_choice;
    doc["parallel_tool_calls"] = false;

    String payload;
    serializeJson(doc, payload);
    String response = _sendHttpRequest(url, "POST", payload);

    JsonDocument responseDoc;
    if (!response.isEmpty()) {
        DeserializationError error = deserializeJson(responseDoc, response);
        if (!error) {
            return RunInfo::fromJson(responseDoc.as<JsonObject>());
        } else {
            Serial.print("JSON-Deserialisierung fehlgeschlagen: ");
            Serial.println(error.c_str());
        }
    }

    return RunInfo();
}

// Retrieves updated info about a run
RunInfo OpenAiApi::retrieveRun(const String& thread_id, const String& run_id) {
    String url = _serverName + "/threads/" + thread_id + "/runs/" + run_id;
    String httpResponse = _sendHttpRequest(url, "GET");

    JsonDocument responseDoc;
    if (!httpResponse.isEmpty()) {
        DeserializationError error = deserializeJson(responseDoc, httpResponse);
        if (!error) {
            return RunInfo::fromJson(responseDoc.as<JsonObject>());
        } else {
            Serial.print("JSON deserialization failed: ");
            Serial.println(error.c_str());
        }
    }

    return RunInfo();
}

// Core HTTP request handler
String OpenAiApi::_sendHttpRequest(const String& url, const String& method, const String& payload) {
    HTTPClient http;
    http.begin(url);
    http.setTimeout(10000);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", "Bearer " + _apiKey);
    http.addHeader("OpenAI-Beta", "assistants=v2");

    int httpResponseCode;

    if (method == "POST") {
        httpResponseCode = http.POST(payload);
    } else if (method == "GET") {
        httpResponseCode = http.GET();
    } else if (method == "DELETE") {
        httpResponseCode = http.sendRequest("DELETE");
    } else {
        Serial.println("Unsupported HTTP method");
        http.end();
        return "";
    }

    String httpresponse = "";
    if (httpResponseCode > 0) {
        if (httpResponseCode == HTTP_CODE_OK || httpResponseCode == HTTP_CODE_CREATED) {
            httpresponse = http.getString();
        } else {
            Serial.println(payload);
            Serial.print("HTTP Error: ");
            Serial.println(httpResponseCode);
            Serial.println(http.getString());
        }
    } else {
        Serial.print("HTTP request failed: ");
        Serial.println(http.errorToString(httpResponseCode).c_str());
        Serial.println(url);
        Serial.println(payload);
    }

    http.end();
    return httpresponse;
}
