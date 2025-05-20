#pragma once

#include "Arduino.h"

#include <FastLED.h>

#include "definitions.h"

struct LightEvent {

  unsigned long timestamp = 0;
  CRGB leds[NUM_LEDS];
};
