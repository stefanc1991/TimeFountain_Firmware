#pragma once

#include <Arduino.h>

class AsyncWebServer;
class AsyncWebServerRequest;

class WebPageHomeClass {
public:
    WebPageHomeClass();

    void begin(String deviceName, AsyncWebServer* server);
    void loop();

private:
    AsyncWebServer* _pServer;
    String _deviceName;
};

extern WebPageHomeClass WebPageHome;