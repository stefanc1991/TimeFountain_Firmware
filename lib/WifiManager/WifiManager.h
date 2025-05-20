#pragma once

#include <Arduino.h>

#define DEFAULT_WIFI_PATIENCE   10000

class WifiManagerClass {
public:
    WifiManagerClass();

    void begin(const char* mdnsHostName, const char* ssidAp, const char* passAp, uint16_t tWifiPatience = DEFAULT_WIFI_PATIENCE);
    void loop();

private:
    enum WifiStatus {
        DISCONNECTED,
        CONNECTED,
        ACCESS_POINT
    };

    WifiStatus _state = DISCONNECTED;

    const char* _ssidAp;
    const char* _passAp;
    const char* _mdnsHostName;

    uint16_t _tWifiPatience;
    unsigned long _tStartWaitingForWifi = 0;
};

extern WifiManagerClass WifiManager;