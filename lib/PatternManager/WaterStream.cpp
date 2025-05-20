#include "WaterStream.h"

// WaterStream constructor without period
WaterStream::WaterStream(uint16_t tUsDutyCycle, uint16_t initialShift) {
  _tUsDutyCycle = tUsDutyCycle;
  _initialShift = initialShift;
}

// Initializes the WaterStream
void WaterStream::init() {
  _tUsNextRegularStateChange = _tUsPeriod * (_initialShift % 360) / 360;
  _tUsNextStateChangeDelay = 0;
  _state = INACTIVE;

  _tUsCurrentVelocity = 0;
  _currentVisibility = 255;
  _currentTopColor = CRGB::Black;
  _currentBotColor = CRGB::Black;

  _iThisMotionPhase = 0;
  _iThisTopColorPhase = 0;
  _iThisBotColorPhase = 0;
  _iThisVisibilityPhase = 0;

  unsigned long currentTime = millis();
  _tMsLastMotionPhaseChange = currentTime;
  _tMsLastVisibilityPhaseChange = currentTime;
  _tMsLastTopColorPhaseChange = currentTime;
  _tMsLastBotColorPhaseChange = currentTime;
}

void WaterStream::setPeriod(uint16_t tUsPeriod) {
  _tUsPeriod = tUsPeriod;
}

// Gets the next state change time
unsigned long WaterStream::getNextStateChangeTime() {
  return _tUsNextRegularStateChange + _tUsNextStateChangeDelay;
}

// Sets the next state change delay
void WaterStream::setNextStateChangeDelay(uint16_t tUsDelay) {
  _tUsNextStateChangeDelay = tUsDelay;
}

// Moves to the next state
void WaterStream::moveOn() {
  if (_state == ACTIVE) {
    _state = INACTIVE;
    _tUsNextRegularStateChange = _tUsNextRegularStateChange + _tUsPeriod - _tUsDutyCycle + _tUsCurrentVelocity;
  }
  else {
    _state = ACTIVE;
    _tUsNextRegularStateChange = _tUsNextRegularStateChange + _tUsDutyCycle;
  }
}

// Checks if the stream is active
bool WaterStream::isActive() {
  return (_state == ACTIVE);
}

// Scales a color based on visibility
CRGB WaterStream::_scaleColor(CRGB color, uint8_t scale) {
  CRGB correctedColor;
  correctedColor.r = scale8_video(color.r, scale);
  correctedColor.g = scale8_video(color.g, scale);
  correctedColor.b = scale8_video(color.b, scale);
  return correctedColor;
}

// Gets the top color
CRGB WaterStream::getTopColor() {
  return (_state == ACTIVE) ? _scaleColor(_currentTopColor, _currentVisibility) : CRGB::Black;
}

// Gets the bottom color
CRGB WaterStream::getBotColor() {
  return (_state == ACTIVE) ? _scaleColor(_currentBotColor, _currentVisibility) : CRGB::Black;
}

// Updates the motion phase
void WaterStream::updateMotion(const unsigned long tMsNow) {
  if (_motionPhases.empty()) return;

  long tMsElapsedTime = tMsNow - _tMsLastMotionPhaseChange;
  uint8_t iNextMotionPhase = (_iThisMotionPhase + 1) % _motionPhases.size();
  const auto& thisPhase = _motionPhases[_iThisMotionPhase];
  const auto& nextPhase = _motionPhases[iNextMotionPhase];

  if (tMsElapsedTime < thisPhase.tMsHold) {
    _tUsCurrentVelocity = thisPhase.velocity;
  }
  else if (tMsElapsedTime < thisPhase.tMsHold + thisPhase.tMsTransition) {
    int deltaVelocity = nextPhase.velocity - thisPhase.velocity;
    _tUsCurrentVelocity = thisPhase.velocity + deltaVelocity * (tMsElapsedTime - thisPhase.tMsHold) / thisPhase.tMsTransition;
  }
  else {
    _tMsLastMotionPhaseChange += thisPhase.tMsHold + thisPhase.tMsTransition;
    _iThisMotionPhase = iNextMotionPhase;
  }
}

// Updates the visibility phase
void WaterStream::updateVisibility(const unsigned long tMsNow) {
  if (_visibilityPhases.empty()) return;

  long tMsElapsedTime = tMsNow - _tMsLastVisibilityPhaseChange;
  uint8_t iNextVisibilityPhase = (_iThisVisibilityPhase + 1) % _visibilityPhases.size();
  const auto& thisPhase = _visibilityPhases[_iThisVisibilityPhase];
  const auto& nextPhase = _visibilityPhases[iNextVisibilityPhase];

  if (tMsElapsedTime < thisPhase.tMsHold) {
    _currentVisibility = thisPhase.brightness;
  }
  else if (tMsElapsedTime < thisPhase.tMsHold + thisPhase.tMsTransition) {
    int deltaBrightness = nextPhase.brightness - thisPhase.brightness;
    _currentVisibility = thisPhase.brightness + deltaBrightness * (tMsElapsedTime - thisPhase.tMsHold) / thisPhase.tMsTransition;
  }
  else {
    _tMsLastVisibilityPhaseChange += thisPhase.tMsHold + thisPhase.tMsTransition;
    _iThisVisibilityPhase = iNextVisibilityPhase;
  }
}

// Updates the top color phase
void WaterStream::updateColorTop(const unsigned long tMsNow) {
  if (_topColorPhases.empty()) return;

  long tMsElapsedTime = tMsNow - _tMsLastTopColorPhaseChange;
  uint8_t iNextColorTopPhase = (_iThisTopColorPhase + 1) % _topColorPhases.size();
  const auto& thisPhase = _topColorPhases[_iThisTopColorPhase];
  const auto& nextPhase = _topColorPhases[iNextColorTopPhase];

  if (tMsElapsedTime < thisPhase.tMsHold) {
    _currentTopColor.r = thisPhase.color.r;
    _currentTopColor.g = thisPhase.color.g;
    _currentTopColor.b = thisPhase.color.b;
  }
  else if (tMsElapsedTime < thisPhase.tMsHold + thisPhase.tMsTransition) {
    int deltaR = (int)nextPhase.color.r - (int)thisPhase.color.r;
    int deltaG = (int)nextPhase.color.g - (int)thisPhase.color.g;
    int deltaB = (int)nextPhase.color.b - (int)thisPhase.color.b;

    _currentTopColor.r = thisPhase.color.r + deltaR * (tMsElapsedTime - thisPhase.tMsHold) / thisPhase.tMsTransition;
    _currentTopColor.g = thisPhase.color.g + deltaG * (tMsElapsedTime - thisPhase.tMsHold) / thisPhase.tMsTransition;
    _currentTopColor.b = thisPhase.color.b + deltaB * (tMsElapsedTime - thisPhase.tMsHold) / thisPhase.tMsTransition;
  }
  else {
    _tMsLastTopColorPhaseChange += thisPhase.tMsHold + thisPhase.tMsTransition;
    _iThisTopColorPhase = iNextColorTopPhase;
  }
}

// Updates the bottom color phase
void WaterStream::updateColorBot(const unsigned long tMsNow) {
  if (_botColorPhases.empty()) return;

  long tMsElapsedTime = tMsNow - _tMsLastBotColorPhaseChange;
  uint8_t iNextColorBotPhase = (_iThisBotColorPhase + 1) % _botColorPhases.size();
  const auto& thisPhase = _botColorPhases[_iThisBotColorPhase];
  const auto& nextPhase = _botColorPhases[iNextColorBotPhase];

  if (tMsElapsedTime < thisPhase.tMsHold) {
    _currentBotColor.r = thisPhase.color.r;
    _currentBotColor.g = thisPhase.color.g;
    _currentBotColor.b = thisPhase.color.b;
  }
  else if (tMsElapsedTime < thisPhase.tMsHold + thisPhase.tMsTransition) {
    int deltaR = (int)nextPhase.color.r - (int)thisPhase.color.r;
    int deltaG = (int)nextPhase.color.g - (int)thisPhase.color.g;
    int deltaB = (int)nextPhase.color.b - (int)thisPhase.color.b;

    _currentBotColor.r = thisPhase.color.r + deltaR * (tMsElapsedTime - thisPhase.tMsHold) / thisPhase.tMsTransition;
    _currentBotColor.g = thisPhase.color.g + deltaG * (tMsElapsedTime - thisPhase.tMsHold) / thisPhase.tMsTransition;
    _currentBotColor.b = thisPhase.color.b + deltaB * (tMsElapsedTime - thisPhase.tMsHold) / thisPhase.tMsTransition;
  }
  else {
    _tMsLastBotColorPhaseChange += thisPhase.tMsHold + thisPhase.tMsTransition;
    _iThisBotColorPhase = iNextColorBotPhase;
  }
}

WaterStream WaterStream::fromStruct(const WaterStreamInfo& info) {
  WaterStream stream(info.dutyCycle, info.initialShift);

  // Direkte Zuweisung der Phasen
  stream._motionPhases = info.motionPhases;
  stream._visibilityPhases = info.visibilityPhases;
  stream._topColorPhases = info.topColorPhases;
  stream._botColorPhases = info.botColorPhases;

  return stream;
}

