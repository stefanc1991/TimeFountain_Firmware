#pragma once

#include <Arduino.h>
#include <vector>
#include <FastLED.h>
#include <ArduinoJson.h>

// Represents the parameters of a color phase
struct ColorPhaseInfo {
    CRGB color;
    uint16_t tMsHold;       // Hold time in milliseconds
    uint16_t tMsTransition; // Transition time in milliseconds

    static ColorPhaseInfo fromJson(JsonObjectConst obj) {
        ColorPhaseInfo info;
        JsonArrayConst rgbArray = obj["RGB"].as<JsonArrayConst>();
        info.color = CRGB(rgbArray[0], rgbArray[1], rgbArray[2]);
        info.tMsHold = obj["HT"].as<uint16_t>();
        info.tMsTransition = obj["TT"].as<uint16_t>();
        return info;
    }

    void toJson(JsonObject obj) const {
        JsonArray rgbArray = obj["RGB"].to<JsonArray>();
        rgbArray.add(color.r);
        rgbArray.add(color.g);
        rgbArray.add(color.b);
        obj["HT"] = tMsHold;
        obj["TT"] = tMsTransition;
    }
};

// Represents the parameters of a motion phase
struct MotionPhaseInfo {
    int16_t velocity;
    uint16_t tMsHold;
    uint16_t tMsTransition;

    static MotionPhaseInfo fromJson(JsonObjectConst obj) {
        MotionPhaseInfo info;
        info.velocity = obj["V"].as<int16_t>();
        info.tMsHold = obj["HT"].as<uint16_t>();
        info.tMsTransition = obj["TT"].as<uint16_t>();
        return info;
    }

    void toJson(JsonObject obj) const {
        obj["V"] = velocity;
        obj["HT"] = tMsHold;
        obj["TT"] = tMsTransition;
    }
};

// Represents the parameters of a visibility phase
struct VisibilityPhaseInfo {
    uint8_t brightness;
    uint16_t tMsHold;
    uint16_t tMsTransition;

    static VisibilityPhaseInfo fromJson(JsonObjectConst obj) {
        VisibilityPhaseInfo info;
        info.brightness = obj["B"].as<uint8_t>();
        info.tMsHold = obj["HT"].as<uint16_t>();
        info.tMsTransition = obj["TT"].as<uint16_t>();
        return info;
    }

    void toJson(JsonObject obj) const {
        obj["B"] = brightness;
        obj["HT"] = tMsHold;
        obj["TT"] = tMsTransition;
    }
};

// Represents the parameters of a waterstream
struct WaterStreamInfo {
    uint16_t dutyCycle;
    uint16_t initialShift;
    std::vector<MotionPhaseInfo> motionPhases;
    std::vector<VisibilityPhaseInfo> visibilityPhases;
    std::vector<ColorPhaseInfo> topColorPhases;
    std::vector<ColorPhaseInfo> botColorPhases;

    static WaterStreamInfo fromJson(JsonObjectConst obj) {
        WaterStreamInfo info;
        info.dutyCycle = obj["DC"].as<uint16_t>();
        info.initialShift = obj["IPS"].as<uint16_t>();

        // Parse all motion phases
        JsonArrayConst mpArray = obj["MP"].as<JsonArrayConst>();
        info.motionPhases.reserve(mpArray.size());
        for (JsonObjectConst mpObj : mpArray) {
            info.motionPhases.push_back(MotionPhaseInfo::fromJson(mpObj));
        }

        // Parse visibility phases
        JsonArrayConst vpArray = obj["VP"].as<JsonArrayConst>();
        info.visibilityPhases.reserve(vpArray.size());
        for (JsonObjectConst vpObj : vpArray) {
            info.visibilityPhases.push_back(VisibilityPhaseInfo::fromJson(vpObj));
        }

        // Parse top color phases
        JsonArrayConst tcpArray = obj["TCP"].as<JsonArrayConst>();
        info.topColorPhases.reserve(tcpArray.size());
        for (JsonObjectConst tcpObj : tcpArray) {
            info.topColorPhases.push_back(ColorPhaseInfo::fromJson(tcpObj));
        }

        // Parse bottom color phases
        JsonArrayConst bcpArray = obj["BCP"].as<JsonArrayConst>();
        info.botColorPhases.reserve(bcpArray.size());
        for (JsonObjectConst bcpObj : bcpArray) {
            info.botColorPhases.push_back(ColorPhaseInfo::fromJson(bcpObj));
        }

        return info;
    }

    void toJson(JsonObject obj) const {
        obj["DC"] = dutyCycle;
        obj["IPS"] = initialShift;

        JsonArray mpArray = obj["MP"].to<JsonArray>();
        for (const auto& mp : motionPhases) {
            JsonObject mpObj = mpArray.add<JsonObject>();
            mp.toJson(mpObj);
        }

        JsonArray vpArray = obj["VP"].to<JsonArray>();
        for (const auto& vp : visibilityPhases) {
            JsonObject vpObj = vpArray.add<JsonObject>();
            vp.toJson(vpObj);
        }

        JsonArray tcpArray = obj["TCP"].to<JsonArray>();
        for (const auto& tcp : topColorPhases) {
            JsonObject tcpObj = tcpArray.add<JsonObject>();
            tcp.toJson(tcpObj);
        }

        JsonArray bcpArray = obj["BCP"].to<JsonArray>();
        for (const auto& bcp : botColorPhases) {
            JsonObject bcpObj = bcpArray.add<JsonObject>();
            bcp.toJson(bcpObj);
        }
    }
};

// Represents the parameters of a pattern made of streams
struct PatternInfo {
    std::vector<WaterStreamInfo> streams;

    static PatternInfo fromJson(JsonObjectConst obj) {
        PatternInfo info;

        JsonArrayConst wsArray = obj["WS"].as<JsonArrayConst>();
        info.streams.reserve(wsArray.size());
        for (JsonObjectConst wsObj : wsArray) {
            WaterStreamInfo waterStream = WaterStreamInfo::fromJson(wsObj);
            info.streams.push_back(waterStream);
        }

        return info;
    }

    void toJson(JsonObject obj) const {
        JsonArray wsArray = obj["WS"].to<JsonArray>();
        for (const auto& stream : streams) {
            JsonObject wsObj = wsArray.add<JsonObject>();
            stream.toJson(wsObj);
        }
    }
};
