#include "LightEventManager.h"

LightEventManager::LightEventManager() {}

void LightEventManager::begin() {
    // Initialisierung, falls notwendig
}

void LightEventManager::startLightControlTask() {
    xTaskCreate(LightControlTaskStaticEntryPoint, "LightControlTask", 2048, this, 1, &_lightControlTaskHandle);
}

void LightEventManager::startLightEventSchedulerTask() {
    xTaskCreate(LightEventSchedulerTaskStaticEntryPoint, "LightEventSchedulerTask", 2048, this, 1, &_lightEventSchedulerTaskHandle);
}

void LightEventManager::LightControlTaskStaticEntryPoint(void* pvParameters) {
    static_cast<LightEventManager*>(pvParameters)->lightControlTask();
    vTaskDelete(NULL);
}

void LightEventManager::LightEventSchedulerTaskStaticEntryPoint(void* pvParameters) {
    static_cast<LightEventManager*>(pvParameters)->lightEventSchedulerTask();
    vTaskDelete(NULL);
}

void LightEventManager::lightControlTask() {
    // Implementierung der Lichtsteuerungslogik
    for (;;) {
        // Task-Logik hier
        vTaskDelay(pdMS_TO_TICKS(1000)); // Beispiel-Verzögerung
    }
}

void LightEventManager::lightEventSchedulerTask() {
    // Implementierung der Logik zur Planung von Lichtereignissen
    for (;;) {
        // Task-Logik hier
        vTaskDelay(pdMS_TO_TICKS(1000)); // Beispiel-Verzögerung
    }
}
