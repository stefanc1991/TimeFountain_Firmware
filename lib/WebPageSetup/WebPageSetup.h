#pragma once

#include <Arduino.h>

class AsyncWebServer;
class IrManagerClass;
class AsyncWebServerRequest;

class WebPageSetupClass {
public:
    WebPageSetupClass();

    void begin(String deviceName, AsyncWebServer* pServer);
    void loop();

    void onUploadStart(std::function<void()> callback);
    void onUploadProgress(std::function<void(size_t current, size_t final)> callback);
    void onUploadEnd(std::function<void(bool success)> callback);


private:
    AsyncWebServer* _pServer;
    IrManagerClass* _pIrManager;

    String _deviceName;
    bool _reboot = false;
    unsigned long _tMsReboot = 0;
    unsigned long _currentProgressSize;

    std::function<void()> _onStartUploadCallback = nullptr;
    std::function<void(size_t current, size_t final)> _onProgressUploadCallback = nullptr;
    std::function<void(bool success)> _onEndUploadCallback = nullptr;

    void _irCodeResetCallbackHandler(AsyncWebServerRequest* request, uint16_t keycode);
};

extern WebPageSetupClass WebPageSetup;