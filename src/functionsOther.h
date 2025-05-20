#pragma once

#include <Arduino.h>

#include <SPI.h>
#include <FastLED.h>

#include "pins.h"
#include "definitions.h"

void flashLeds(struct CRGB* leds);