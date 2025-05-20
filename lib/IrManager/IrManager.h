#pragma once

#include <Arduino.h>
#include <vector>

#define IR_DEBOUNCE_DELAY 200
#define INTERVAL_MS_DECODE_LOOP 10

// Represents a single IR key with its code and action
struct IrKey {
    const String keyFunction;
    uint16_t keyCode;
    std::function<void()> callback;
};

class IrManagerClass {
public:
    IrManagerClass();

    void begin(uint8_t receivePin, const String filename);
    void addIrKey(const String name, uint16_t keyCode, const std::function<void()> callback);
    void initializeIrCodeReset(const String name, const std::function<void(uint16_t keyCode)> callback);
    bool isIrCodeResetInitialized();

    void decodeIrSignalTask();        // Decoding loop
    void processIrSignalTask();       // Processes decoded IR data

    static void decodeIrSignalTaskWrapper(void *pvParameters);   // FreeRTOS-compatible wrapper
    static void processIrSignalTaskWrapper(void *pvParameters);  // FreeRTOS-compatible wrapper

    static IrManagerClass* instance;  // Pointer to the singleton instance (used in static wrappers)

private:
    static TaskHandle_t _hDecodeIrSignalTask;
    static TaskHandle_t _hProcessIrSignalTask;

    std::vector<IrKey> _irKeys;
    String _filename;
    uint8_t _receivePin;

    IrKey* _pIrCodeResetTarget = nullptr;
    std::function<void(uint16_t keyCode)> _irCodeResetCallback;

    void _synchronizeIRCodesFromFile();       // Load IR codes from persistent storage
    void _writeIRCodesToFile() const;         // Save IR codes to file
    IrKey* _getKeyByCode(uint16_t keyCodeToCheck);
    IrKey* _getKeyByFunction(const String functionToCheck);
};

// Global instance
extern IrManagerClass IrManager;
