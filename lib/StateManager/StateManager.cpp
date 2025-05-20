#include "StateManager.h"

// Singleton instance
StateManager* StateManager::instance = nullptr;
TaskHandle_t StateManager::_loopTaskHandle = nullptr;

StateManager::StateManager(StateManagementType stateManagingType) {
    _stateManagementType = stateManagingType;
    StateManager::instance = this;

    xTaskCreate(loopTaskWrapper, "LoopTask", 4096, NULL, 0, &_loopTaskHandle);
}

State* StateManager::addState() {
    State* pNewState = new State(this);
    _pStates.push_back(pNewState);
    return pNewState;
}

void StateManager::activateState(State* pState) {
    if (pState == nullptr)
        return;

    // Ensure state exists in manager
    auto it = std::find(_pStates.begin(), _pStates.end(), pState);
    if (it == _pStates.end()) {
        Serial.println("Error: State not found");
        return;
    }

    // Request permission to activate from new state
    if (!pState->_onRequestCallback || pState->_onRequestCallback(_pActiveState)) {
        _switchStateProcedure(pState);
    }
}

bool StateManager::isStateActive(State* pState) const {
    return _pActiveState == pState;
}

void StateManager::nextState() {
    if (_pStates.empty() || _pActiveState == nullptr)
        return;

    if (_stateManagementType == INDEPENDENT) {
        Serial.println("nextState() can not be used for an INDEPENDENT StateManager!");
        return;
    }

    auto it = std::find(_pStates.begin(), _pStates.end(), _pActiveState);

    if (it != _pStates.end()) {
        size_t iActiveState = std::distance(_pStates.begin(), it);

        if (_stateManagementType == CYCLIC) {
            _pStates[(iActiveState + 1) % _pStates.size()]->activate();
        }
        else if (_stateManagementType == CLAMPED) {
            _pStates[min(iActiveState + 1, _pStates.size() - 1)]->activate();
        }
    }
}

void StateManager::previousState() {
    if (_pStates.empty() || _pActiveState == nullptr)
        return;

    if (_stateManagementType == INDEPENDENT) {
        Serial.println("nextState() can not be used for INDEPENDENT StateManager!");
        return;
    }

    auto it = std::find(_pStates.begin(), _pStates.end(), _pActiveState);

    if (it != _pStates.end()) {
        size_t iActiveState = std::distance(_pStates.begin(), it);

        if (_stateManagementType == CYCLIC) {
            _pStates[(iActiveState - 1 + _pStates.size()) % _pStates.size()]->activate();
        }
        else if (_stateManagementType == CLAMPED) {
            _pStates[max(int(iActiveState) - 1, 0)]->activate();
        }
    }
}

bool StateManager::restoreLastState() {
    if (_pSavedLastState) {
        _pSavedLastState->activate();
        return true;
    }
    return false;
}

void StateManager::deactivate() {
    _switchStateProcedure(nullptr);
}

// Background state loop
void StateManager::loopTask() {
    while (true) {
        vTaskDelay(INTERVAL_MS_STATE_LOOP / portTICK_PERIOD_MS);

        if (_pActiveState && _pActiveState->_onLoopCallback) {
            _pActiveState->_onLoopCallback();
        }
    }
}

// Static wrapper to call instance method
void StateManager::loopTaskWrapper(void* pvParameters) {
    StateManager::instance->loopTask();
}

// Handles exit from current state and entry into new one
void StateManager::_switchStateProcedure(State* pNewState) {
    if (_pActiveState && _pActiveState->_onEndCallback) {
        _pActiveState->_onEndCallback(pNewState);
    }

    _pSavedLastState = _pActiveState;
    _pActiveState = pNewState;

    if (_pActiveState && _pActiveState->_onStartCallback) {
        _pActiveState->_onStartCallback(_pSavedLastState);
    }
}
