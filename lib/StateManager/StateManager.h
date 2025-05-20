#pragma once

#include <Arduino.h>
#include <vector>
#include <String>

#include "State.h"

#define INTERVAL_MS_STATE_LOOP 100

class State;

enum StateManagementType {
    CYCLIC,
    CLAMPED,
    INDEPENDENT
};

class StateManager {
public:
    StateManager(StateManagementType stateManagingType);

    State* addState();

    void activateState(State* pState);
    bool isStateActive(State* pState) const;

    void nextState();
    void previousState();
    bool restoreLastState();

    void deactivate();

    void loopTask();                                 // Called by task
    static void loopTaskWrapper(void* pvParameters); // FreeRTOS-compatible wrapper

    static StateManager* instance; // Singleton pointer for task access

private:
    static TaskHandle_t _loopTaskHandle;

    State* _pActiveState = nullptr;
    State* _pSavedLastState = nullptr;
    StateManagementType _stateManagementType;
    std::vector<State*> _pStates;

    void _switchStateProcedure(State* pNewState);
};
