#include "Pattern.h"

Pattern::Pattern() {
    // Default constructor
}

Pattern::Pattern(uint16_t period) {
    this->_tUsPeriod = period;
}

void Pattern::init() {
    // Initialize all water streams
    for (auto& stream : streams) {
        stream.init();
    }
}

void Pattern::setPeriod(uint16_t period) {
    _tUsPeriod = period;
    for (auto& stream : streams) {
        stream.setPeriod(period);
    }
}

void Pattern::updateStreamProperties() {
    const unsigned long nowTimeMs = millis();

    // Update visual and motion properties of each stream
    for (auto& stream : streams) {
        stream.updateColorTop(nowTimeMs);
        stream.updateColorBot(nowTimeMs);
        stream.updateMotion(nowTimeMs);
        stream.updateVisibility(nowTimeMs);
    }
}

WaterStream* Pattern::getNextChangingStream() {
    WaterStream* pNextChangingStream;
    unsigned long tMaxTimestamp = 0;
    unsigned long tNextLightEvent = ULONG_MAX;

    // Find the latest change time across all streams
    for (auto& stream : streams) {
        unsigned long tTmpTimestamp = stream.getNextStateChangeTime();
        if (tTmpTimestamp > tMaxTimestamp) {
            tMaxTimestamp = tTmpTimestamp;
        }
    }

    // Find the stream with the next upcoming change (closest in time)
    for (auto& stream : streams) {
        unsigned long tTmpTimestamp = stream.getNextStateChangeTime();
        if (tTmpTimestamp < tNextLightEvent &&
            tMaxTimestamp - tTmpTimestamp < ULONG_MAX / 2) {
            pNextChangingStream = &stream;
            tNextLightEvent = tTmpTimestamp;
        }
    }

    return pNextChangingStream;
}

CRGB Pattern::getTotalTopColor() {
    CRGB totalTopColor = CRGB::Black;

    // Sum top colors of all active streams
    for (auto& stream : streams) {
        if (stream.isActive()) {
            totalTopColor += stream.getTopColor();
        }
    }

    return totalTopColor;
}

CRGB Pattern::getTotalBotColor() {
    CRGB totalBotColor = CRGB::Black;

    // Sum bottom colors of all active streams
    for (auto& stream : streams) {
        if (stream.isActive()) {
            totalBotColor += stream.getBotColor();
        }
    }

    return totalBotColor;
}

Pattern Pattern::fromStruct(const PatternInfo& patternInfo) {
    Pattern pattern;

    // Build pattern from struct representation
    for (const auto& streamInfo : patternInfo.streams) {
        WaterStream stream = WaterStream::fromStruct(streamInfo);
        pattern.streams.push_back(stream);
    }

    return pattern;
}

bool Pattern::hasStreams() const {
    return !streams.empty();
}
