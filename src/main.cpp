/*
 * ----------------------------------------------------------------------------
 * Project:       AI-supported Time Fountain
 * Description:   Software for my mostly 3D printed DIY indoor fountain, 
 *                that creates the issusion of curved waterstreams 
 *                flowing down in slow motion, standing still or flowing upwards
 *                (supports IR control, OTA updates and LLM-based pattern generation)
 *
 * Platform:      ESP32 / Arduino Framework
 * Author:        Stefan Conrad
 * Created:       17.05.2025
 * Comments:      Minimal due to private project ;)
 * ----------------------------------------------------------------------------
 */

#include "main.h"

void setup() {

  totalHeap = esp_get_free_heap_size(); // debugging

  Serial.begin(115200);
  Serial.println("Start");

  LittleFsHelper::begin();

  pinMode(TB6612_STBY_PIN, OUTPUT);
  pinMode(TB6612_POWER_PIN, OUTPUT);
  // pinMode(POWER_CHECK_PIN, INPUT_PULLDOWN);
  digitalWrite(TB6612_STBY_PIN, HIGH);
  digitalWrite(TB6612_POWER_PIN, HIGH);

  SPI.begin();
  SPI.beginTransaction(SPISettings(SPI_CLOCK, MSBFIRST, SPI_MODE0));

  // the SettingsManager will search for these settings in the file and create them with defaukt values if not present 
  SettingsManager.addSetting(SETTING_MAGNET_POWER, DEFAULT_MAGNET_POWER);
  SettingsManager.addSetting(SETTING_VIBRATION_PERIOD, DEFAULT_VIBRATION_PERIOD);
  SettingsManager.begin(SETTINGS_FILE);

  vibrationPeriod = SettingsManager.getSetting(SETTING_VIBRATION_PERIOD).toInt();
  int magnetPower = SettingsManager.getSetting(SETTING_MAGNET_POWER).toInt();
  // Serial.println(magnetPower);

  queueLightEvents = xQueueCreate(2*MAX_NUM_LIGHT_EVENTS, sizeof(struct LightEvent)); 

  xTaskCreatePinnedToCore(magnetControlTask, "MagnetControlTask", 3000, NULL, 25, &magnetControlTaskHandler, 1);
  xTaskCreatePinnedToCore(lightControlTask, "LightControlTask", 3000, NULL, 24, &lightControlTaskHandler, 1);
  xTaskCreatePinnedToCore(lightEventSchedularTask, "EventSchedularTask", 3000, NULL, 23, &eventSchedularTaskHandler, 1); // needs priority >= Wifi task, cause otherwise results in flickers

  pTimerMagnet = timerBegin(0, 80, true);
  timerAttachInterrupt(pTimerMagnet, &timerInterruptMagnet, true);
  timerAlarmWrite(pTimerMagnet, vibrationPeriod / 2, true);

  pTimerLight = timerBegin(2, 80, true); // timer 2 chosen since timer 0 and 1 are in the same hardware group
  timerAttachInterrupt(pTimerLight, &timerInterruptLightEvent, true);
  timerAlarmWrite(pTimerLight, 100000, true);

  WifiManager.begin(MDNS_HOST_NAME, AP_SSID, AP_PASS);

  // WiFi.onEvent(onWiFiConnected, ARDUINO_EVENT_WIFI_STA_CONNECTED);
  // WiFi.onEvent(onWiFiDisconnected, ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
  // WiFi.onEvent(onApConnected, ARDUINO_EVENT_WIFI_AP_STACONNECTED);

  // the IrManager will look for these keys in the file and create them with default values if not present
  IrManager.addIrKey(IR_KEY_PLAY, IR_KEY_DEFAULT_PLAY, onClickIrKeyPlay);
  IrManager.addIrKey(IR_KEY_MENU, IR_KEY_DEFAULT_MENU, onClickIrMenu);
  IrManager.addIrKey(IR_KEY_UP, IR_KEY_DEFAULT_UP, onClickIrKeyUp);
  IrManager.addIrKey(IR_KEY_DOWN, IR_KEY_DEFAULT_DOWN, onClickIrKeyDown);
  IrManager.addIrKey(IR_KEY_RIGHT, IR_KEY_DEFAULT_RIGHT, onClickIrKeyRight);
  IrManager.addIrKey(IR_KEY_LEFT, IR_KEY_DEFAULT_LEFT, onClickIrKeyLeft);
  IrManager.addIrKey(IR_KEY_OK, IR_KEY_DEFAULT_OK, onClickIrKeyOk);
  IrManager.begin(IR_RECEIVE_PIN, IR_KEYS_FILE);

  // StateManager is defined with Independent mode in main.h (states are not sorted)
  pStatePlay = stateManager.addState();
  pStatePlay->onStart(onStartStatePlay);
  pStatePlay->onLoop(onLoopStatePlay);
  pStatePlay->onEnd(onEndStatePlay);

  pStateConfig = stateManager.addState();
  pStateConfig->onStart(onStartStateConfig);
  pStateConfig->onLoop(onLoopStateConfig);
  pStateConfig->onEnd(onEndStateConfig);

  pStateStandby = stateManager.addState();
  pStateStandby->onLoop(onLoopStateStandby);
  
  // Webpages need to be defined before server.begin()
  WebPageHome.begin(DEVICE_NAME, &server);

  WebPageSetup.begin(DEVICE_NAME, &server);
  WebPageSetup.onUploadStart(onUploadStart);
  WebPageSetup.onUploadProgress(onUploadProgress);
  WebPageSetup.onUploadEnd(onUploadEnd);

  WebPageAiChat.begin(DEVICE_NAME, &server);
  WebPageAiChat.onUserMessage(aiChatMessageCallback);
  WebPageAiChat.onUndoLastChange(undoLastChangesCallback);
  WebPageAiChat.onSaveChanges(saveChangesCallback);
  WebPageAiChat.setBackupFile(PLAY_PATTERNS_FILE);

  WebPageHome.begin(DEVICE_NAME, &server);

  server.begin();

  // two seperate Pattern Managers allow flexible handling of PLAY and CONFIG mode
  patternManagerPlay.setPeriod(vibrationPeriod);
  patternManagerPlay.loadPatterns(PLAY_PATTERNS_FILE);
  patternManagerConfig.setPeriod(vibrationPeriod);
  patternManagerConfig.loadPatterns(CONFIG_PATTERNS_FILE);

  pump.setPower(DEFAULT_PUMP_POWER);
  elMagnet.setPower(magnetPower);
  // elMagnet.setPower(100);

  // start to trigger initial light event interruft
  timerAlarmEnable(pTimerMagnet);

  pStatePlay->activate();
}

void loop() {

  vTaskDelay(INTERVAL_MS_LOOP / portTICK_PERIOD_MS); // not sure why but although low priority the loop task causes flickers if not delayed

  elMagnet.update();

  WifiManager.loop();
  WebPageSetup.loop();
  WebPageAiChat.loop();
}

void restartLights() {

  // handles a break in the light interrupts while setting up the new pattern to avoid flickering
  portENTER_CRITICAL(&timerMux);

  timerAlarmDisable(pTimerLight);

  resetSchedularFlag = true;
  xQueueReset(queueLightEvents);

  timerAlarmWrite(pTimerLight, 1000, true);
  timerAlarmEnable(pTimerLight);

  portEXIT_CRITICAL(&timerMux);
}

// this task is the core of the fountain, it determines the next light event and adds it to the queue
// light events are changes in the LEDs color caused by a Waterstream entering an active or inactive phase
void lightEventSchedularTask(void* pvParameters) {

  static unsigned long tLastLightEvent = 0;

  while (true) {

    // Wait for valid pattern or space on queue for new Light Events
    if (!uxQueueSpacesAvailable(queueLightEvents) || !pActivePattern || !pActivePattern->hasStreams()) {
      if (pActivePattern) {
        pActivePattern->updateStreamProperties();
      }

      vTaskDelay(INTERVAL_MS_EVENT_SCHEDULAR / portTICK_PERIOD_MS);
      continue;
    }

    if (resetSchedularFlag) {
      tLastLightEvent = 0;
      resetSchedularFlag = false;
    }

    // finds next light event in the active pattern (each waterstream manages its own timestamps)
    // timestamps are relative to schedular time (tLastLightEvent) not controller time
    WaterStream* pNextChangingStream = pActivePattern->getNextChangingStream();
    unsigned long tNextLightEvent = pNextChangingStream->getNextStateChangeTime();

    // makes the WaterStream jump to the next state and update timestamp
    // as the schedular task is planning events nothing here is directly affecting the hardware (that happens in LightControlTask())
    pNextChangingStream->moveOn();

    // if the time difference between two light events is shorter than the programming time of the LEDs the next task needs to be dealyed
    // To do: If the next event is close enough to the lsat one they should be merged and executed in one light event
    uint16_t tNextLightEventDelay = max(0, MIN_TIME_BETWEEN_EVENTS - (int)(tNextLightEvent - tLastLightEvent));

    tNextLightEvent += tNextLightEventDelay;
    pNextChangingStream->setNextStateChangeDelay(tNextLightEventDelay); // must be set after moveOn()

    // construct new LightEvent
    LightEvent lightevent;
    lightevent.timestamp = tNextLightEvent;

    // the color to be programmed is composed of all active water streams after the next Waterstream changed activity
    CRGB newTopColor = pActivePattern->getTotalTopColor();
    CRGB newBotColor = pActivePattern->getTotalBotColor();

    // better viewer experience if three top and bottom LEDs have a constant color
    for (int i = 0; i < 3; i++) {
      lightevent.leds[i] = newTopColor;
      lightevent.leds[NUM_LEDS - 1 - i] = newBotColor;
    }

    fill_gradient_RGB(lightevent.leds, 3, newTopColor, NUM_LEDS - 4, newBotColor);

    //send Light Event to Queue
    xQueueSend(queueLightEvents, (void*)&lightevent, (TickType_t)0);

    tLastLightEvent = tNextLightEvent;
  }
}

// task gets triggered by timerInterruptLightEvent()
void lightControlTask(void* pvParameters) {

  LightEvent thisLightEvent;
  LightEvent nextLightEvent;

  while (true) {

    // waits for a trigger from interrupt and checks if enough light events are queued
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    if (!xQueueReceive(queueLightEvents, &thisLightEvent, (TickType_t)0) || !xQueuePeek(queueLightEvents, &nextLightEvent, (TickType_t)1000)) {
      timerAlarmWrite(pTimerLight, 1000, true);
      continue;
    }
    
    // SPI based programming must not be interrupted
    portENTER_CRITICAL(&timerMux);
    flashLeds(thisLightEvent.leds);
    portEXIT_CRITICAL(&timerMux);
    
    uint32_t tUsTimeToNextEvent = nextLightEvent.timestamp - thisLightEvent.timestamp;

    portENTER_CRITICAL(&timerMux);
    timerAlarmWrite(pTimerLight, tUsTimeToNextEvent, true);  // This does not restart the timer. It just sets a new alarm time
    portEXIT_CRITICAL(&timerMux);
  }
}

// task gets triggered by timerInterruptMagnet()
void magnetControlTask(void* pvParameters) {

  while (true) {

    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    elMagnet.togglePolerization();
  }
}
