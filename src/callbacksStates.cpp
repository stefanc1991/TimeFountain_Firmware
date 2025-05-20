#include "callbacksStates.h"

void onStartStatePlay(State* pLastState) {
    
    if (patternManagerPlay.selectPattern(0, pActivePattern)) {
        Serial.println("Play");
   
        restartLights();

        if (pLastState != pStateConfig) {
            pump.setState(ON);
            delay(500);
        }

        elMagnet.setState(ON);
    }
}

void onLoopStatePlay() {

}

void onEndStatePlay(State* pNextState) {
    
    elMagnet.setState(OFF);
    while (!elMagnet.isTargetPowerReached()) {
        delay(500);
    }

    if (pNextState != pStateConfig) {
        pump.setState(OFF);
        timerAlarmDisable(pTimerLight);
    }
}


void onStartStateConfig(State* pLastState) {

    if (patternManagerConfig.selectPattern(0, pActivePattern)) {
        restartLights();

        if (pLastState != pStateConfig) {
            elMagnet.setState(ON);
            pump.setState(ON);
        }
    }
}

void onLoopStateConfig() {

}

void onEndStateConfig(State* pNextState) {

    elMagnet.setState(OFF);
    while (!elMagnet.isTargetPowerReached()) {
        delay(500);
    }

    SettingsManager.saveSettings();

    if (pNextState != pStatePlay) {
        pump.setState(OFF);
        timerAlarmDisable(pTimerLight);
    }
}

void onLoopStateStandby() {

    static unsigned long previousMillis = 0;
    static uint8_t startIndex = 0;

    unsigned long currentMillis = millis();
    CRGB leds[NUM_LEDS];
    
    if (currentMillis - previousMillis >= 30) {
        previousMillis = currentMillis;

        fill_rainbow(leds, NUM_LEDS, startIndex, 2);
        startIndex += 1;

        flashLeds(leds);
    }
}