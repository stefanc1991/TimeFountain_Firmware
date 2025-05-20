#pragma once

#include <Arduino.h>

#define MAGNET_PWM_CH   2
#define MAGNET_PWM_RES  8
#define MAGNET_PWM_FREQ 100000

#define POWER_INCREMENT 10

#define setPinHigh(b) (GPIO.out_w1ts = ((uint32_t)1 << b))  // much faster than digitalWrite, not working for pin 32,33 due to different register
#define setPinLow(b)  (GPIO.out_w1tc = ((uint32_t)1 << b))

enum Polarization {
  NORTH,
  SOUTH
};

class ElectroMagnet {

public:
  ElectroMagnet(uint8_t pinCtrl1, uint8_t pinCtrl2, uint8_t pinPWM);
  bool isTargetPowerReached() const;
  void togglePolerization();
  void setState(bool state);
  // bool getState();
  void setPower(uint8_t value);
  uint8_t getPower();
  void update();

private:
  uint8_t _pinCtrl1;
  uint8_t _pinCtrl2;
  uint8_t _pinPWM;
  bool _state = false;
  uint8_t _currentPower = 0;
  uint8_t _maxPower = 0;
  uint8_t _targetPower = 0;
  unsigned long _tmsLastUpdate = 0;
  Polarization _pol;

  float _powerIncrementPerMs = 0;
  uint16_t _powerTransitionTime = 1000;
  
  void _setCurrentPower(uint8_t value);
  void _init();
};
