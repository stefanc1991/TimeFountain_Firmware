#include "WebPageAiChat.h"
#include "WebPageSetup.h"

WebPageAiChatClass::WebPageAiChatClass() {}

void WebPageAiChatClass::begin(String deviceName, AsyncWebServer* server) {
    _deviceName = deviceName;
    _pServer = server;

    // Redirect root to /aichat
    _pServer->on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
        request->redirect("/aichat");
    });

    // Serve static UI files
    _pServer->on("/aichat", HTTP_GET, [](AsyncWebServerRequest* request) {
        String fileContent = LittleFsHelper::readFile("/aichat.html");
        request->send(200, "text/html", fileContent);
    });

    _pServer->on("/aichat.css", HTTP_GET, [](AsyncWebServerRequest* request) {
        String fileContent = LittleFsHelper::readFile("/aichat.css");
        request->send(200, "text/css", fileContent);
    });

    _pServer->on("/aichat.js", HTTP_GET, [](AsyncWebServerRequest* request) {
        String fileContent = LittleFsHelper::readFile("/aichat.js");
        request->send(200, "text/javascript", fileContent);
    });

    // Return device name as plain text
    _pServer->on("/getdevicename", HTTP_GET, [this](AsyncWebServerRequest* request) {
        request->send(200, "text/plain", _deviceName);
    });

    // Return backup file content or error
    _pServer->on("/getbackupfile", HTTP_GET, [this](AsyncWebServerRequest* request) {
        if (_backupFilePath.isEmpty()) {
            request->send(404, "text/plain", "No Backup file set");
        } else if (!LittleFsHelper::fileExists(_backupFilePath)) {
            request->send(404, "text/plain", "File not found");
        } else {
            String fileContent = LittleFsHelper::readFile(_backupFilePath);
            if (fileContent.length() > 0) {
                String filename = _getFilenameFromPath(_backupFilePath);
                AsyncWebServerResponse* response = request->beginResponse(200, "application/json", fileContent);
                response->addHeader("Content-Disposition", "attachment; filename=" + filename);
                request->send(response);
            } else {
                request->send(500, "text/plain", "Failed to read file content");
            }
        }
    });

    // Trigger save callback
    _pServer->on("/savechanges", HTTP_GET, [this](AsyncWebServerRequest* request) {
        if (_saveChangesCallback) {
            _saveChangesCallback();
            request->send(200, "text/plain", "All changes saved");
        } else {
            request->send(200, "text/plain", "No Save function available");
        }
    });

    // Trigger undo callback
    _pServer->on("/undolastchange", HTTP_GET, [this](AsyncWebServerRequest* request) {
        if (_undoLastChangeCallback) {
            if (_undoLastChangeCallback()) {
                request->send(200, "text/plain", "Last change successfully reversed");
            } else {
                request->send(200, "text/plain", "No more saved changes");
            }
        } else {
            request->send(200, "text/plain", "No Undo function available");
        }
    });

    // WebSocket message handler
    ws.onEvent([this](AsyncWebSocket* server, AsyncWebSocketClient* client, AwsEventType type, void* arg, uint8_t* data, size_t len) {
        if (type == WS_EVT_DATA) {
            _pCurrentClient = client;
            handleWebSocketMessage(client, arg, data, len);
        }
    });

    _pServer->addHandler(&ws);
}

void WebPageAiChatClass::handleWebSocketMessage(AsyncWebSocketClient* client, void* arg, uint8_t* data, size_t len) {
    AwsFrameInfo* info = (AwsFrameInfo*)arg;

    // Accept only full text frames
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
        _userMessage = String((char*)data).substring(0, len);
    }
}

void WebPageAiChatClass::sendResponseToClient(String message) {
    if (_pCurrentClient && _pCurrentClient->status() == WS_CONNECTED) {
        _pCurrentClient->text(message);
    }
}

// Register callback for new user message via WebSocket
void WebPageAiChatClass::onUserMessage(std::function<void(String)> callback) {
    _userMessageCallback = callback;
}

// Register callback for saving state
void WebPageAiChatClass::onSaveChanges(std::function<void()> callback) {
    _saveChangesCallback = callback;
}

// Register callback for undo operation
void WebPageAiChatClass::onUndoLastChange(std::function<bool()> callback) {
    _undoLastChangeCallback = callback;
}

void WebPageAiChatClass::setBackupFile(String backupFilePath) {
    _backupFilePath = backupFilePath;
}

void WebPageAiChatClass::loop() {
    if (!_userMessage.isEmpty()) {
        _userMessageCallback(_userMessage);
        _userMessage = "";
    }

    ws.cleanupClients();
}

// Extract filename from full path
String WebPageAiChatClass::_getFilenameFromPath(const String& path) {
    int lastSlashIndex = path.lastIndexOf('/');
    if (lastSlashIndex != -1 && lastSlashIndex < path.length() - 1) {
        return path.substring(lastSlashIndex + 1);
    }
    return path;
}

// Global instance
WebPageAiChatClass WebPageAiChat;
