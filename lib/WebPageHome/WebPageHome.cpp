#include "WebPageHome.h"

#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include "LittleFsHelper.h"

WebPageHomeClass::WebPageHomeClass() {}

void WebPageHomeClass::begin(String deviceName, AsyncWebServer* server) {
    _deviceName = deviceName;
    _pServer = server;

    // Redirect root to /home
    _pServer->on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
        request->redirect("/home");
    });

    // Serve home page
    _pServer->on("/home", HTTP_GET, [](AsyncWebServerRequest* request) {
        String fileContent = LittleFsHelper::readFile("/home.html");
        request->send(200, "text/html", fileContent);
    });

    // Serve CSS
    _pServer->on("/home.css", HTTP_GET, [](AsyncWebServerRequest* request) {
        String fileContent = LittleFsHelper::readFile("/home.css");
        request->send(200, "text/css", fileContent);
    });

    // Serve JavaScript
    _pServer->on("/home.js", HTTP_GET, [](AsyncWebServerRequest* request) {
        String fileContent = LittleFsHelper::readFile("/home.js");
        request->send(200, "text/javascript", fileContent);
    });

    // Provide device name
    _pServer->on("/getdevicename", HTTP_GET, [this](AsyncWebServerRequest* request) {
        request->send(200, "text/plain", _deviceName);
    });
}

void WebPageHomeClass::loop() {
    // Placeholder for future periodic logic (if needed)
}

// Global instance
WebPageHomeClass WebPageHome;
