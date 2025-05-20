#pragma once

#include <Arduino.h>

class LightEventManager {

public:
    LightEventManager();
    void begin();

    void startLightControlTask();         // Starts the task managing physical light control
    void startLightEventSchedulerTask();  // Starts the task handling timed light events

private:
    // FreeRTOS-compatible task entry points
    static void LightControlTaskStaticEntryPoint(void* pvParameters);
    static void LightEventSchedulerTaskStaticEntryPoint(void* pvParameters);

    // Actual task logic
    void lightControlTask();
    void lightEventSchedulerTask();

    TaskHandle_t _lightControlTaskHandle = NULL;
    TaskHandle_t _lightEventSchedulerTaskHandle = NULL;
};
