#pragma once

#include <Arduino.h>

#define PUMP_PWM_CH     1
#define PUMP_PWM_RES    8
#define PUMP_PWM_FREQ   100

class Pump {

  public:
    Pump(uint8_t pinCtrl, uint8_t pinGND, uint8_t pinPWM, uint8_t power);
    void setState(bool state);
    bool getState();
    void setPower(uint8_t value);
    uint8_t getPower();
    
  private:
    uint8_t _pinCtrl;
    uint8_t _pinGND;
    uint8_t _pinPWM;
    bool _state;
    uint8_t _power;

    void _init();

};