#include "WifiManager.h"

#include <WiFi.h>
#include <ESPmDNS.h>

WifiManagerClass::WifiManagerClass() {}

void WifiManagerClass::begin(const char* mdnsHostName, const char* ssidAp, const char* passAp, uint16_t tWifiPatience) {
  _mdnsHostName = mdnsHostName;
  _ssidAp = ssidAp;
  _passAp = passAp;
  _tWifiPatience = tWifiPatience;

  // Handle successful Wi-Fi connection
  WiFi.onEvent([this](WiFiEvent_t event, WiFiEventInfo_t info) {
    if (_state == CONNECTED)
      return;

    delay(1000);  // Allow time for stable connection
    Serial.print("Connected to: ");
    Serial.println(WiFi.SSID());
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());

    MDNS.begin(_mdnsHostName);
    MDNS.addService("http", "tcp", 80);

    _state = CONNECTED;
  }, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_CONNECTED);

  // Handle disconnection
  WiFi.onEvent([this](WiFiEvent_t event, WiFiEventInfo_t info) {
    if (_state == DISCONNECTED)
      return;

    delay(100);
    Serial.println("Wifi disconnected");

    MDNS.end();
    _state = DISCONNECTED;
  }, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED);

  WiFi.mode(WIFI_STA);
  WiFi.begin();

  _tStartWaitingForWifi = millis();

  Serial.println("Connecting to Wifi...");
}

void WifiManagerClass::loop() {
  switch (_state) {
    case DISCONNECTED:
      // If timeout exceeded and still not connected, start fallback AP
      if (WiFi.status() != WL_CONNECTED && millis() - _tStartWaitingForWifi > _tWifiPatience) {
        Serial.println("No Wifi connection possible. Start Access Point...");

        WiFi.mode(WIFI_AP);
        WiFi.softAP(_ssidAp, _passAp);

        Serial.println("Access Point started");
        Serial.print("SSID: ");
        Serial.println(_ssidAp);
        Serial.print("IP: ");
        Serial.println(WiFi.softAPIP());

        _state = ACCESS_POINT;
      }
      break;

    case CONNECTED:
      // Nothing to do
      break;

    case ACCESS_POINT:
      // Nothing to do
      break;
  }
}

// Global instance
WifiManagerClass WifiManager;
