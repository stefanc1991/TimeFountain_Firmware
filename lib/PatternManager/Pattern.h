#pragma once

#include <Arduino.h>
#include <vector>
#include "Waterstream.h"
#include "PatternInfoStructs.h"

class Pattern {
  
  public:
    Pattern();
    Pattern(uint16_t period);

    void init();
    void setPeriod(uint16_t period);
    void updateStreamProperties();
    WaterStream* getNextChangingStream();
    CRGB getTotalTopColor();
    CRGB getTotalBotColor();
    
    bool hasStreams() const;

    static Pattern fromStruct(const PatternInfo& patternInfo);

  private:
    std::vector<WaterStream> streams;
    uint16_t _tUsPeriod = 13000; // Default period
};