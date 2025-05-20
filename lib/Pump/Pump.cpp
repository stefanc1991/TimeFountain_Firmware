
#include "Pump.h"

Pump::Pump(uint8_t pinCtrl, uint8_t pinGND, uint8_t pinPWM, uint8_t power) {
  _pinCtrl = pinCtrl;
  _pinGND = pinGND;
  _pinPWM = pinPWM;
  _power = power;
  _init();
}

void Pump::_init() {
  pinMode(_pinCtrl, OUTPUT);
  pinMode(_pinGND, OUTPUT);
  pinMode(_pinPWM, OUTPUT);
  
  digitalWrite(_pinGND, LOW);

  ledcSetup(PUMP_PWM_CH, PUMP_PWM_FREQ, PUMP_PWM_RES);
  ledcAttachPin(_pinPWM, PUMP_PWM_CH);

  setState(LOW);
}

void Pump::setState(bool state) {
  if(_state == state) return;
  
  if (state) {
    Serial.println("PUMP ON");
  }
  else {
    Serial.println("PUMP OFF");
  }
  _state = state;
  digitalWrite(_pinCtrl, state);
}

bool Pump::getState() {
  return(_state);
}

void Pump::setPower(uint8_t value) {
  _power = value;
  ledcWrite(PUMP_PWM_CH, value);
}

uint8_t Pump::getPower() {
  return(_power);
}
