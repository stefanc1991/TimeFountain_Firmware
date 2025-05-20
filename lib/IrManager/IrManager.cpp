#include "IrManager.h"

#define SEND_PWM_BY_TIMER  // Avoids warning in IRremote.h

#include <ArduinoJson.h>
#include "LittleFsHelper.h"

// #define NO_LED_FEEDBACK_CODE
#include <IRremote.hpp>

// Static member initialization
IrManagerClass* IrManagerClass::instance = nullptr;
TaskHandle_t IrManagerClass::_hDecodeIrSignalTask = nullptr;
TaskHandle_t IrManagerClass::_hProcessIrSignalTask = nullptr;

IrManagerClass::IrManagerClass() {
    IrManagerClass::instance = this;
}

void IrManagerClass::begin(uint8_t receivePin, const String filename) {
    _receivePin = receivePin;
    _filename = filename;

    IrReceiver.begin(_receivePin);

    // Create FreeRTOS tasks
    xTaskCreate(decodeIrSignalTaskWrapper, "DecodeIrSignalTask", 4096, NULL, 5, &_hDecodeIrSignalTask);
    xTaskCreate(processIrSignalTaskWrapper, "ProcessIrSignalTask", 4096, NULL, 0, &_hProcessIrSignalTask);

    _synchronizeIRCodesFromFile();
}

void IrManagerClass::addIrKey(const String function, uint16_t keyCode, const std::function<void()> callback) {
    _irKeys.push_back({ function, keyCode, callback });
}

void IrManagerClass::initializeIrCodeReset(const String function, const std::function<void(uint16_t keyCode)> callback) {
    _pIrCodeResetTarget = _getKeyByFunction(function);
    _irCodeResetCallback = callback;
}

bool IrManagerClass::isIrCodeResetInitialized() {
    return _pIrCodeResetTarget != nullptr;
}

void IrManagerClass::decodeIrSignalTask() {
    static unsigned long lastKeyPressTime = 0;

    while (true) {
        vTaskDelay(INTERVAL_MS_DECODE_LOOP / portTICK_PERIOD_MS);

        unsigned long msTimeNow = millis();

        if (IrReceiver.decode()) {
            // Debounce to avoid repeated signal triggers
            if (msTimeNow - lastKeyPressTime > IR_DEBOUNCE_DELAY) {
                xTaskNotifyGive(_hProcessIrSignalTask);
            }
            lastKeyPressTime = msTimeNow;
        }

        IrReceiver.resume();
    }
}

void IrManagerClass::processIrSignalTask() {
    while (true) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        uint16_t keyCode = IrReceiver.decodedIRData.command;

        // Update IR code mapping if in reset mode
        if (_pIrCodeResetTarget) {
            _pIrCodeResetTarget->keyCode = keyCode;
            _pIrCodeResetTarget = nullptr;
            _writeIRCodesToFile();
            _irCodeResetCallback(keyCode);
            _irCodeResetCallback = NULL;
            continue;
        }

        // Trigger associated callback if key is recognized
        const IrKey* key = _getKeyByCode(keyCode);
        if (key) {
            key->callback();
        }
    }
}

// FreeRTOS task wrappers
void IrManagerClass::decodeIrSignalTaskWrapper(void* pvParameters) {
    IrManagerClass::instance->decodeIrSignalTask();
}

void IrManagerClass::processIrSignalTaskWrapper(void* pvParameters) {
    IrManagerClass::instance->processIrSignalTask();
}

// Loads IR codes from a file into the _irKeys vector
void IrManagerClass::_synchronizeIRCodesFromFile() {
    JsonDocument doc;

    if (LittleFsHelper::fileExists(_filename)) {
        String fileContent = LittleFsHelper::readFile(_filename);
        DeserializationError error = deserializeJson(doc, fileContent);

        if (error) {
            Serial.println("Failed to deserialize IR file");
            return;
        }

        JsonArray arr = doc["irKeys"].as<JsonArray>();

        if (!arr.isNull()) {
            for (auto& key : _irKeys) {
                for (JsonObject obj : arr) {
                    if (key.keyFunction == obj["function"].as<String>()) {
                        key.keyCode = obj["code"];
                        break;
                    }
                }
            }
        }
    }

    _writeIRCodesToFile();
}

// Writes current IR key mappings to file
void IrManagerClass::_writeIRCodesToFile() const {
    JsonDocument newDoc;
    JsonArray keyArray = newDoc["irKeys"].to<JsonArray>();

    for (auto& key : _irKeys) {
        JsonObject newKey = keyArray.add<JsonObject>();
        newKey["function"] = key.keyFunction;
        newKey["code"] = key.keyCode;
    }

    String content = "";
    serializeJson(newDoc, content);

    LittleFsHelper::writeFile(_filename, content);
}

// Finds key by IR code
IrKey* IrManagerClass::_getKeyByCode(uint16_t keyCodeToCheck) {
    for (auto& key : _irKeys) {
        if (key.keyCode == keyCodeToCheck) {
            return &key;
        }
    }
    return nullptr;
}

// Finds key by function name
IrKey* IrManagerClass::_getKeyByFunction(const String keyFunctionToCheck) {
    for (auto& key : _irKeys) {
        if (key.keyFunction == keyFunctionToCheck) {
            return &key;
        }
    }
    return nullptr;
}

// Global instance
IrManagerClass IrManager;
