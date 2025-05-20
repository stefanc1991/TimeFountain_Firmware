#include "State.h"

#include "StateManager.h"

State::State(StateManager* pManager) : _pManager(pManager) {}

void State::activate() {
    _pManager->activateState(this);
}

bool State::isActive() {
    return _pManager->isStateActive(this);
}

void State::onRequest(const std::function<bool(State* pCurrentState)> callback) {
    _onRequestCallback = callback;
}

void State::onStart(const std::function<void(State* pLastState)> callback) {
    _onStartCallback = callback;
}

void State::onLoop(const std::function<void()> callback) {
    _onLoopCallback = callback;
}

void State::onEnd(const std::function<void(State* pNextState)> callback) {
    _onEndCallback = callback;
}

