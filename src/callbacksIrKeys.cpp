#include "callbacksIrKeys.h"

// Infrared-Keys available for this project

void onClickIrKeyPlay() {
  if (pStateConfig->isActive() || pStateStandby->isActive()) {
    pStatePlay->activate();
  }
  else {
    pStateStandby->activate();
  }
}

void onClickIrMenu() {
  if (pStatePlay->isActive() || pStateStandby->isActive()) {
    pStateConfig->activate();
  }
  else {
    pStatePlay->activate();
  }
}

void onClickIrKeyUp() {
  if (pStatePlay->isActive()) {
    //   brightness += BRIGHTNESS_INCREMENT;
    //   lightLayerManager.setBrightness(brightness);
  }
  else if (pStateConfig->isActive()) {
    vibrationPeriod -= VIBRATION_PERIOD_INCREMENT;
    Serial.println(MICRO_SECONDS_PER_SECOND / vibrationPeriod);

    //TODO: needs to be redone (timerAlarm should not be set here!)
    timerAlarmWrite(pTimerMagnet, vibrationPeriod / 2, true);
    patternManagerPlay.setPeriod(vibrationPeriod);
    patternManagerConfig.setPeriod(vibrationPeriod);
    SettingsManager.setSetting(SETTING_VIBRATION_PERIOD, String(vibrationPeriod));
  }
}

void onClickIrKeyDown() {
  if (pStatePlay->isActive()) {
    //   brightness -= BRIGHTNESS_INCREMENT;
    //   lightLayerManager.setBrightness(brightness);
  }
  else if (pStateConfig->isActive()) {
    vibrationPeriod += VIBRATION_PERIOD_INCREMENT;
    Serial.println(MICRO_SECONDS_PER_SECOND / vibrationPeriod);
    
    //TODO: needs to be redone (timerAlarm should not be set here!)
    timerAlarmWrite(pTimerMagnet, vibrationPeriod / 2, true);
    patternManagerPlay.setPeriod(vibrationPeriod);
    patternManagerConfig.setPeriod(vibrationPeriod);
    SettingsManager.setSetting(SETTING_VIBRATION_PERIOD, String(vibrationPeriod));
  }
}

void onClickIrKeyRight() {
  if (pStatePlay->isActive()) {
    if (patternManagerPlay.nextPattern(pActivePattern)) {
      restartLights();
    }
  }
  else if (pStateConfig->isActive()) {
    //TODO: Code needs to be reworked (this should not be done in keyCallbacks)
    uint8_t currentMagnetPower = elMagnet.getPower();
    int incrementMagnetPower = 0.1 * currentMagnetPower;
    incrementMagnetPower = max(1, incrementMagnetPower);
    uint8_t newMagnetPower = min(255, currentMagnetPower + incrementMagnetPower);
    elMagnet.setPower(newMagnetPower);

    SettingsManager.setSetting(SETTING_MAGNET_POWER, String(newMagnetPower));
  }
}

void onClickIrKeyLeft() {
  if (pStatePlay->isActive()) {
    if (patternManagerPlay.previousPattern(pActivePattern)) {
      restartLights();
    }
  }
  else if (pStateConfig->isActive()) {
    //TODO: Code needs to be reworked (this should not be done in keyCallbacks)
    uint8_t currentMagnetPower = elMagnet.getPower();
    int incrementMagnetPower = 0.1 * currentMagnetPower;
    incrementMagnetPower = max(1, incrementMagnetPower);
    uint8_t newMagnetPower = max(0, currentMagnetPower - incrementMagnetPower);
    elMagnet.setPower(newMagnetPower);

    SettingsManager.setSetting(SETTING_MAGNET_POWER, String(newMagnetPower));
  }
}

void onClickIrKeyOk() {
  if (pStateConfig->isActive() || pStateStandby->isActive()) {
    pStatePlay->activate();
  }
  else {
    pStateStandby->activate();
  }
}
