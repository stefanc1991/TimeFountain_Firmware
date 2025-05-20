#pragma once

#define DEBUG

#ifdef DEBUG
    #include <Arduino.h>
    #define debug(x) Serial.print(x)
    #define debugln(x) Serial.println(x)
    #define debugf(fmt, ...) Serial.printf(fmt, __VA_ARGS__)
#else
    #define debug(x)
    #define debugln(x)
    #define debugf(fmt, ...)
#endif