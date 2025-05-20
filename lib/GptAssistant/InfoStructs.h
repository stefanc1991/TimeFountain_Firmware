#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include <vector>

// Represents a callable tool the assistant can use
struct Tool {
    std::string name;
    std::string description;
    std::function<void(const std::string&, const std::string&)> function;

    Tool(const std::string& name, 
         const std::string& description, 
         std::function<void(const std::string&, const std::string&)> func)
        : name(name), description(description), function(func) {}
};

// Token usage stats
struct Usage {
    int prompt_tokens;
    int completion_tokens;
    int total_tokens;
};

// Strategy for truncating input messages
struct TruncationStrategy {
    String type;
    JsonObject last_messages;
};

// Encapsulates a function call with arguments
struct Function {
    String name;
    String arguments;

    static Function fromJson(const JsonObject& obj) {
        Function func;
        func.name = obj["name"].as<String>();
        func.arguments = obj["arguments"].as<String>();
        return func;
    }
};

// Describes a single tool call made during a run
struct ToolCall {
    String id;
    String type;
    Function function;

    static ToolCall fromJson(const JsonObject& obj) {
        ToolCall call;
        call.id = obj["id"].as<String>();
        call.type = obj["type"].as<String>();
        call.function = Function::fromJson(obj["function"].as<JsonObject>());
        return call;
    }
};

// Tool call wrapper used in required actions
struct SubmitToolOutputs {
    std::vector<ToolCall> tool_calls;

    static SubmitToolOutputs fromJson(const JsonObject& obj) {
        SubmitToolOutputs outputs;
        JsonArray callsArray = obj["tool_calls"].as<JsonArray>();
        for (JsonObject callObj : callsArray) {
            outputs.tool_calls.push_back(ToolCall::fromJson(callObj));
        }
        return outputs;
    }
};

// Action required to continue run (e.g. tool response)
struct RequiredAction {
    String type;
    SubmitToolOutputs submit_tool_outputs;

    static RequiredAction fromJson(const JsonObject& obj) {
        RequiredAction action;
        action.type = obj["type"].as<String>();
        if (obj["submit_tool_outputs"].is<JsonObject>()) {
            action.submit_tool_outputs = SubmitToolOutputs::fromJson(obj["submit_tool_outputs"].as<JsonObject>());
        }
        return action;
    }
};

// Run metadata and state information
struct RunInfo {
    String id;
    String object;
    unsigned long created_at;
    String assistant_id;
    String thread_id;
    String status;
    unsigned long started_at;
    unsigned long expires_at;
    unsigned long cancelled_at;
    unsigned long failed_at;
    unsigned long completed_at;
    String last_error;
    String model;
    String instructions;
    std::vector<String> tools;
    JsonObject metadata;
    String incomplete_details;
    Usage usage;
    double temperature;
    double top_p;
    int max_prompt_tokens;
    int max_completion_tokens;
    TruncationStrategy truncation_strategy;
    String response_format;
    String tool_choice;
    bool parallel_tool_calls;
    RequiredAction required_action;

    static RunInfo fromJson(const JsonObject& obj) {
        RunInfo info;
        info.id = obj["id"].as<String>();
        info.object = obj["object"].as<String>();
        info.created_at = obj["created_at"].as<unsigned long>();
        info.assistant_id = obj["assistant_id"].as<String>();
        info.thread_id = obj["thread_id"].as<String>();
        info.status = obj["status"].as<String>();
        info.started_at = obj["started_at"].as<unsigned long>();
        info.expires_at = obj["expires_at"].isNull() ? 0 : obj["expires_at"].as<unsigned long>();
        info.cancelled_at = obj["cancelled_at"].isNull() ? 0 : obj["cancelled_at"].as<unsigned long>();
        info.failed_at = obj["failed_at"].isNull() ? 0 : obj["failed_at"].as<unsigned long>();
        info.completed_at = obj["completed_at"].isNull() ? 0 : obj["completed_at"].as<unsigned long>();
        info.last_error = obj["last_error"].isNull() ? "" : obj["last_error"].as<String>();
        info.model = obj["model"].as<String>();
        info.instructions = obj["instructions"].isNull() ? "" : obj["instructions"].as<String>();

        JsonArray toolsArray = obj["tools"].as<JsonArray>();        
        for (JsonObject toolObj : toolsArray) {
            String toolString;
            serializeJson(toolObj, toolString);
            info.tools.push_back(toolString);
        }

        info.metadata = obj["metadata"].as<JsonObject>();
        info.incomplete_details = obj["incomplete_details"].isNull() ? "" : obj["incomplete_details"].as<String>();

        if (obj["usage"].is<JsonObject>()) {            
            JsonObject usageObj = obj["usage"].as<JsonObject>();
            info.usage.prompt_tokens = usageObj["prompt_tokens"].as<int>();
            info.usage.completion_tokens = usageObj["completion_tokens"].as<int>();
            info.usage.total_tokens = usageObj["total_tokens"].as<int>();
        }

        info.temperature = obj["temperature"].as<double>();
        info.top_p = obj["top_p"].as<double>();
        info.max_prompt_tokens = obj["max_prompt_tokens"].isNull() ? 0 : obj["max_prompt_tokens"].as<int>();
        info.max_completion_tokens = obj["max_completion_tokens"].isNull() ? 0 : obj["max_completion_tokens"].as<int>();

        JsonObject truncationObj = obj["truncation_strategy"].as<JsonObject>();
        info.truncation_strategy.type = truncationObj["type"].as<String>();
        info.truncation_strategy.last_messages = truncationObj["last_messages"].isNull() ? JsonObject() : truncationObj["last_messages"].as<JsonObject>();

        info.response_format = obj["response_format"].as<String>();
        info.tool_choice = obj["tool_choice"].as<String>();
        info.parallel_tool_calls = obj["parallel_tool_calls"].as<bool>();

        if (obj["required_action"].is<JsonObject>()) {
            info.required_action = RequiredAction::fromJson(obj["required_action"].as<JsonObject>());
        }

        return info;
    }
};

// Text content with optional annotations
struct TextContent {
    String value;
    std::vector<String> annotations;
};

// Represents a message content item (type = text)
struct Content {
    String type;
    TextContent text;
};

// Chat message metadata and content
struct MessageInfo {
    String id;
    String object;
    unsigned long created_at;
    String thread_id;
    String role;
    std::vector<Content> content;
    String assistant_id;
    String run_id;
    std::vector<JsonObject> attachments;
    JsonObject metadata;

    static MessageInfo fromJson(const JsonObject& obj) {
        MessageInfo info;
        info.id = obj["id"].as<String>();
        info.object = obj["object"].as<String>();
        info.created_at = obj["created_at"].as<unsigned long>();
        info.thread_id = obj["thread_id"].as<String>();
        info.role = obj["role"].as<String>();

        JsonArray contentArray = obj["content"].as<JsonArray>();
        for (JsonObject contentObj : contentArray) {
            Content content;
            content.type = contentObj["type"].as<String>();
            JsonObject textObj = contentObj["text"].as<JsonObject>();
            content.text.value = textObj["value"].as<String>();

            JsonArray annotationsArray = textObj["annotations"].as<JsonArray>();
            for (JsonVariant annotation : annotationsArray) {
                content.text.annotations.push_back(annotation.as<String>());
            }

            info.content.push_back(content);
        }

        info.assistant_id = obj["assistant_id"].as<String>();
        info.run_id = obj["run_id"].as<String>();

        JsonArray attachmentsArray = obj["attachments"].as<JsonArray>();
        for (JsonObject attachmentObj : attachmentsArray) {
            info.attachments.push_back(attachmentObj);
        }

        info.metadata = obj["metadata"].as<JsonObject>();

        return info;
    }
};

// Thread instance used in assistant conversations
struct ThreadInfo {
    String id;
    String object;
    unsigned long created_at;
    JsonObject metadata;

    static ThreadInfo fromJson(const JsonObject& obj) {
        ThreadInfo info;
        info.id = obj["id"].as<String>();
        info.object = obj["object"].as<String>();
        info.created_at = obj["created_at"].as<unsigned long>();
        info.metadata = obj["metadata"].as<JsonObject>();
        return info;
    }
};

// Metadata for the assistant configuration
struct AssistantInfo {
    String id;
    String object;
    unsigned long created_at;
    String name;
    String description;
    String model;
    String instructions;
    std::vector<String> tools;
    JsonObject metadata;
    double top_p;
    double temperature;
    String response_format;

    static AssistantInfo fromJson(const JsonObject& obj) {
        AssistantInfo info;
        info.id = obj["id"].as<String>();
        info.object = obj["object"].as<String>();
        info.created_at = obj["created_at"].as<unsigned long>();
        info.name = obj["name"].as<String>();
        info.description = obj["description"].isNull() ? "" : obj["description"].as<String>();
        info.model = obj["model"].as<String>();
        info.instructions = obj["instructions"].as<String>();

        JsonArray toolsArray = obj["tools"].as<JsonArray>();        
        for (JsonObject toolObj : toolsArray) {
            String toolString;
            serializeJson(toolObj, toolString);
            info.tools.push_back(toolString);
        }

        info.metadata = obj["metadata"].as<JsonObject>();
        info.top_p = obj["top_p"].as<double>();
        info.temperature = obj["temperature"].as<double>();
        info.response_format = obj["response_format"].as<String>();

        return info;
    }
};
