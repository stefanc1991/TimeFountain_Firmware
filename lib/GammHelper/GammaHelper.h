#pragma once

#include <Arduino.h>

namespace GammaHelper {

    // Generates an LED gamma-correction table
    void calcGammeTable(float gamma) {

        int max_in = 255;
        int max_out = 255;

        Serial.println("const uint8_t PROGMEM gamma[] = {");
        for (int i = 0; i <= max_in; i++) {
            if (i > 0) Serial.print(',');
            if ((i & 15) == 0) Serial.println();
            Serial.printf("%3d",
                (int)(pow((float)i / (float)max_in, gamma) * max_out + 0.5));
        }
        Serial.println(" };");
    }
}
