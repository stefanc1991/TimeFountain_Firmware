#include "ElectroMagnet.h"

ElectroMagnet::ElectroMagnet(uint8_t pinCtrl1, uint8_t pinCtrl2, uint8_t pinPWM) {
  _pinCtrl1 = pinCtrl1;
  _pinCtrl2 = pinCtrl2;
  _pinPWM = pinPWM;
  _init();
}

void ElectroMagnet::_init() {
  // Configure control and PWM pins
  pinMode(_pinCtrl1, OUTPUT);
  pinMode(_pinCtrl2, OUTPUT);
  pinMode(_pinPWM, OUTPUT);

  // Set up PWM channel
  ledcSetup(MAGNET_PWM_CH, MAGNET_PWM_FREQ, MAGNET_PWM_RES);
  ledcAttachPin(_pinPWM, MAGNET_PWM_CH);

  _pol = NORTH;

  setState(LOW);
  _setCurrentPower(0);
}

void ElectroMagnet::update() {
  unsigned long tmsNow = millis();

  // No change needed
  if (_currentPower == _targetPower) {
    _tmsLastUpdate = tmsNow;
    return;
  }

  // Immediate change if no ramping
  if (_powerIncrementPerMs == 0) {
    _setCurrentPower(_targetPower);
    return;
  }

  // Calculate how much power should change since last update
  uint8_t change = _powerIncrementPerMs * (tmsNow - _tmsLastUpdate);

  if (change > 0) {
    _tmsLastUpdate = tmsNow;

    // Ramp power up or down toward target
    if (_currentPower < _targetPower) {
      uint8_t newPower = min(int(_currentPower) + change, int(_targetPower));
      _setCurrentPower(newPower);
    }
    else if (_currentPower > _targetPower) {
      uint8_t newPower = max(int(_currentPower) - change, int(_targetPower));
      _setCurrentPower(newPower);
    }
  }
}

bool ElectroMagnet::isTargetPowerReached() const {
  return _currentPower == _targetPower;
}

void ElectroMagnet::togglePolerization() {
  // Swap active control pin to reverse polarity
  if (_pol == NORTH) {
    setPinHigh(_pinCtrl1);
    setPinLow(_pinCtrl2);
    _pol = SOUTH;
  }
  else {
    setPinLow(_pinCtrl1);
    setPinHigh(_pinCtrl2);
    _pol = NORTH;
  }
}

void ElectroMagnet::setState(bool state) {
  if (state == _state)
    return;

  _state = state;

  // Set new target power based on state
  _targetPower = state ? _maxPower : 0;

  // Calculate rate of power change (per ms)
  _powerIncrementPerMs = abs(int(_targetPower) - int(_currentPower)) / float(_powerTransitionTime);
  _tmsLastUpdate = millis();
}

void ElectroMagnet::_setCurrentPower(uint8_t value) {
  _currentPower = value;
  ledcWrite(MAGNET_PWM_CH, value);
}

void ElectroMagnet::setPower(uint8_t value) {
  _maxPower = value;
  if (_state) {
    _targetPower = _maxPower;
  }
}

uint8_t ElectroMagnet::getPower() {
  return(_maxPower);
}
