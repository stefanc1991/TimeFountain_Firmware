#pragma once

#include <Arduino.h>

class StateManager;

class State {
    friend class StateManager;

public:
    State(StateManager* pManager);

    void activate();
    bool isActive();

    // Called before switching to this state. Return true to allow the transition.
    void onRequest(const std::function<bool(State* pCurrentState)> callback);

    // Called once when this state becomes active.
    void onStart(const std::function<void(State* pLastState)> callback);

    // Called repeatedly while this state remains active.
    void onLoop(const std::function<void()> callback);

    // Called once when this state is deactivated.
    void onEnd(const std::function<void(State* pNextState)> callback);

private:
    // Callbacks passed on creation to make the states customizable
    std::function<bool(State* pCurrentState)> _onRequestCallback;
    std::function<void(State* pLastState)> _onStartCallback;
    std::function<void()> _onLoopCallback;
    std::function<void(State* pNextState)> _onEndCallback;

    StateManager* _pManager;
};
