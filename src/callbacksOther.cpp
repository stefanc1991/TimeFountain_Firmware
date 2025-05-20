#include "callbacksOther.h"

// void onWiFiConnected(WiFiEvent_t event, WiFiEventInfo_t info) {
//   server.begin();
// }

// void onWiFiDisconnected(WiFiEvent_t event, WiFiEventInfo_t info) {
//   server.end();
// }

// void onApConnected(WiFiEvent_t event, WiFiEventInfo_t info) {
//   server.begin();
// }

void onUploadStart() {
  stateManager.deactivate();
  timerAlarmDisable(pTimerLight);
}

void onUploadProgress(size_t current, size_t final) {

  CRGB leds[NUM_LEDS];

  current = min(current, final);

  size_t numBrightLeds = map(current, 0, final + 1, 0, NUM_LEDS);
  uint8_t colorFracGreen = map(current, 0, final + 1, 0, 256);
  uint8_t colorFracRed = 255 - colorFracGreen;

  fill_solid(leds, NUM_LEDS, CRGB::Black);

  for (size_t i = NUM_LEDS - 1; i >= NUM_LEDS - numBrightLeds; i--) {
    leds[i] = CRGB(colorFracRed, colorFracGreen, 0);
  }

  flashLeds(leds);
}

void onUploadEnd(bool success) {
  stateManager.restoreLastState();
}
