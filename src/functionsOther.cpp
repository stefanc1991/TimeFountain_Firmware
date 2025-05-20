#include "functionsOther.h"


void flashLeds(struct CRGB* leds) {

  // flush with zeros (according to APA102c standard)
  SPI.transfer32(0x00000000);

  // program APA102c LED strip with 24 Bit per LED
  for (int i = 0; i < NUM_LEDS; i++) {
    SPI.transfer32((0b11111111 << 24) | ((uint32_t)leds[i].b << 16) | ((uint32_t)leds[i].g << 8) | ((uint32_t)leds[i].r));
  }

  // flush with ones (according to APA102c standard)
  SPI.transfer32(0xFFFFFFFF);
  SPI.transfer32(0xFFFFFFFF);
}
