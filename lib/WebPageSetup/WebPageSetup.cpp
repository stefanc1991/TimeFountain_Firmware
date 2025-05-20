#include "WebPageSetup.h"

#include <AsyncTCP.h>
#include <Update.h>
#include <ESPAsyncWebServer.h>

#include "LittleFsHelper.h"
#include "IrManager.h"

WebPageSetupClass::WebPageSetupClass() {}

void WebPageSetupClass::begin(String deviceName, AsyncWebServer* server) {
    _deviceName = deviceName;
    _pServer = server;

    // Redirect root to /setup
    _pServer->on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
        request->redirect("/setup");
    });

    // Serve setup page
    _pServer->on("/setup", HTTP_GET, [](AsyncWebServerRequest* request) {
        String fileContent = LittleFsHelper::readFile("/setup.html");
        request->send(200, "text/html", fileContent);
    });

    _pServer->on("/setup.css", HTTP_GET, [](AsyncWebServerRequest* request) {
        String fileContent = LittleFsHelper::readFile("/setup.css");
        request->send(200, "text/css", fileContent);
    });

    _pServer->on("/setup.js", HTTP_GET, [](AsyncWebServerRequest* request) {
        String fileContent = LittleFsHelper::readFile("/setup.js");
        request->send(200, "text/javascript", fileContent);
    });

    // Schedule reboot
    _pServer->on("/reset", HTTP_GET, [this](AsyncWebServerRequest* request) {
        int delay = 500;
        if (request->hasArg("delay")) {
            delay = request->arg("ssid").toInt();  // Possible error: using "ssid" instead of "delay"
        }
        request->send(200, "text/plain", "Rebooting...");
        _tMsReboot = millis() + delay;
        _reboot = true;
    });

    _pServer->on("/getdevicename", HTTP_GET, [this](AsyncWebServerRequest* request) {
        request->send(200, "text/plain", _deviceName);
    });

    // Serve stored IR codes
    _pServer->on("/getircodesjson", HTTP_GET, [](AsyncWebServerRequest* request) {
        String fileContent = LittleFsHelper::readFile("/ir_keys.json");
        request->send(200, "text/json", fileContent);
    });

    // Trigger IR learning
    _pServer->on("/getnewcode", HTTP_GET, [this](AsyncWebServerRequest* request) {
        if (!IrManager.isIrCodeResetInitialized()) {
            String keyFunction = request->arg("keyFunction");
            IrManager.initializeIrCodeReset(keyFunction.c_str(), [this, request](uint16_t keyCode) {
                _irCodeResetCallbackHandler(request, keyCode);
            });
        }
    });

    // Handle firmware update upload
    _pServer->on("/update", HTTP_POST, [this](AsyncWebServerRequest* request) {
        AsyncWebServerResponse* response = request->beginResponse(200, "text/html", !Update.hasError() ? "Update successful. Rebooting..." : "Update failed");
        response->addHeader("Connection", "close");
        request->send(response);

        if (!Update.hasError()) {
            _tMsReboot = millis() + 2000;
            _reboot = true;
        }
    }, [this](AsyncWebServerRequest* request, String filename, size_t index, uint8_t* data, size_t len, bool final) {
        if (!index) {
            Serial.printf("Update Start: %s\n", filename.c_str());
            if (!Update.begin((ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000)) {
                Update.printError(Serial);
            }
            if (_onStartUploadCallback) _onStartUploadCallback();
        }

        if (len && !Update.hasError()) {
            if (Update.write(data, len) != len) {
                Update.printError(Serial);
            }
        }

        _currentProgressSize += len;
        if (_onProgressUploadCallback) {
            _onProgressUploadCallback(_currentProgressSize, request->contentLength());
        }

        if (final) {
            if (Update.end(true)) {
                Serial.printf("Update Success: %uB\n", index + len);
            } else {
                Update.printError(Serial);
            }
        }
    });

    // Handle static file uploads
    _pServer->on("/upload", HTTP_POST, [this](AsyncWebServerRequest* request) {
        if (_onEndUploadCallback) _onEndUploadCallback(true);

        AsyncWebServerResponse* response = request->beginResponse(200, "text/html", !Update.hasError() ? "Upload successful" : "Upload failed");
        response->addHeader("Connection", "close");
        request->send(response);
    }, [this](AsyncWebServerRequest* request, String filename, size_t index, uint8_t* data, size_t len, bool final) {
        if (!index) {
            Serial.printf("Upload Start: %s\n", filename.c_str());
            if (LittleFsHelper::fileExists("/" + filename)) {
                LittleFsHelper::deleteFile("/" + filename);
            }
            if (_onStartUploadCallback) _onStartUploadCallback();
        }

        if (len) {
            LittleFsHelper::appendRawDataToFile("/" + filename, data, len);
            _currentProgressSize += len;

            if (_onProgressUploadCallback) {
                _onProgressUploadCallback(_currentProgressSize, request->contentLength());
            }
        }

        if (final) {
            Serial.printf("Upload Success: %uB\n", index + len);
            if (_onEndUploadCallback) {
                _onEndUploadCallback(true);
            }
        }
    });

    // Handle Wi-Fi credentials and restart
    _pServer->on("/connect", HTTP_POST, [this](AsyncWebServerRequest* request) {
        String ssid, pass;

        if (request->hasArg("ssid")) {
            ssid = request->arg("ssid");
        }
        if (request->hasArg("password")) {
            pass = request->arg("password");
        }

        if (!ssid.length() || !pass.length()) {
            request->send(500, "text/plain", "Invalid credentials!");
            Serial.println("Invalid credentials!");
            return;
        }

        request->send(200, "text/plain", "Credentials received. Rebooting...");
        Serial.println("Credentials received. Rebooting...");

        if (WiFi.status() == WL_CONNECTED) {
            WiFi.disconnect();
        }
        if (WiFi.getMode() != WIFI_STA) {
            WiFi.mode(WIFI_STA);
        }

        WiFi.persistent(true);
        WiFi.begin(ssid.c_str(), pass.c_str());

        _tMsReboot = millis() + 2000;
        _reboot = true;
    });
}

void WebPageSetupClass::loop() {
    if (_reboot && millis() > _tMsReboot) {
        Serial.println(F("Esp reboot ..."));
        delay(500);
        ESP.restart();
    }
}

// Register upload lifecycle callbacks
void WebPageSetupClass::onUploadStart(std::function<void()> callback) {
    _onStartUploadCallback = callback;
}

void WebPageSetupClass::onUploadProgress(std::function<void(size_t current, size_t final)> callback) {
    _onProgressUploadCallback = callback;
}

void WebPageSetupClass::onUploadEnd(std::function<void(bool success)> callback) {
    _onEndUploadCallback = callback;
}

// Respond to IR code request with captured keycode
void WebPageSetupClass::_irCodeResetCallbackHandler(AsyncWebServerRequest* request, uint16_t keycode) {
    if (!request) return;

    AsyncWebServerResponse* response = request->beginResponse(200, "text/plain", String(keycode));
    request->send(response);
}

// Global instance
WebPageSetupClass WebPageSetup;
