#pragma once

#include <Arduino.h>
#include <vector>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include "LittleFsHelper.h"
#include "GptAssistant.h"

class AsyncWebServer;
class AsyncWebServerRequest;


class WebPageAiChatClass {
public:
    WebPageAiChatClass();

    void begin(String deviceName, AsyncWebServer* server);
    void loop();

    void onUserMessage(std::function<void(String)> callback);
    void onSaveChanges(std::function<void()> callback);
    void onUndoLastChange(std::function<bool()> callback);
    
    void setBackupFile(String filePath);

    void sendResponseToClient(String message);
    void handleWebSocketMessage(AsyncWebSocketClient* client, void* arg, uint8_t* data, size_t len);

private:

    String _getFilenameFromPath(const String& path);

    String _deviceName;
    AsyncWebServer* _pServer;

    AsyncWebSocket ws{ "/ws" };
    AsyncWebSocketClient* _pCurrentClient;
    String _userMessage = "";
    String _backupFilePath = "";
    std::function<void(String)> _userMessageCallback;
    std::function<void()> _saveChangesCallback;
    std::function<bool()> _undoLastChangeCallback;
    
};

extern WebPageAiChatClass WebPageAiChat;
