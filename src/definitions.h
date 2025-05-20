#pragma once

#include "Arduino.h"

#define MDNS_HOST_NAME                  "timefountain"            // Host name
#define DEVICE_NAME                     "Time Fountain"           // Device name

#define AP_SSID                         "TimeFountain-AP"         // SSID Name
#define AP_PASS                         NULL                      // SSID Password - Set to NULL to have an open AP
#define AP_CHANNEL                      10                        // WiFi Channel number between 1 and 13
#define AP_HIDE_SSID                    false                     // To disable SSID broadcast -> SSID will not appear in a basic WiFi scan
#define AP_MAX_CONNECTIONS              1                         // Maximum simultaneous connected clients on the AP


#define WIFI_PATIENCE                   20000

// Delays
#define INTERVAL_MS_LOOP                10
#define INTERVAL_MS_EVENT_SCHEDULAR     1

// Defaults
#define DEFAULT_BRIGHTNESS              255
#define DEFAULT_PUMP_POWER              255

#define MAGNET_POWER_INCREMENT          10
#define VIBRATION_PERIOD_INCREMENT      100

// General
#define NUM_LEDS                        30
#define SPI_CLOCK                       24000000
#define MIN_TIME_BETWEEN_EVENTS         220                 // has to be longer than lightControlTask ( ~ 100 us ); Increase if light jumps appear or last Led flickers
#define MAX_NUM_LIGHT_EVENTS            10                  // nobody needs more than 10 Streams...
#define SEND_PWM_BY_TIMER

// Settings
#define SETTING_MAGNET_POWER            "MAGNET_POWER"
#define SETTING_VIBRATION_PERIOD        "VIBRATION_PERIOD"

#define DEFAULT_MAGNET_POWER            "0"
#define DEFAULT_VIBRATION_PERIOD        "15000"                 // use this to tune wave length (not pump pmw)

#define SETTINGS_FILE                   "/settings.json"

// Button Constants
#define IR_KEY_PLAY                     "IR_KEY_PLAY"
#define IR_KEY_MENU                     "IR_KEY_MENU"
#define IR_KEY_UP                       "IR_KEY_UP"
#define IR_KEY_DOWN                     "IR_KEY_DOWN"
#define IR_KEY_RIGHT                    "IR_KEY_RIGHT"
#define IR_KEY_LEFT                     "IR_KEY_LEFT"
#define IR_KEY_OK                       "IR_KEY_OK"

#define IR_KEY_DEFAULT_PLAY             0x5E
#define IR_KEY_DEFAULT_MENU             0x02
#define IR_KEY_DEFAULT_UP               0x0B
#define IR_KEY_DEFAULT_DOWN             0x0D
#define IR_KEY_DEFAULT_RIGHT            0x07
#define IR_KEY_DEFAULT_LEFT             0x08
#define IR_KEY_DEFAULT_OK               0x5d

#define IR_KEYS_FILE                    "/ir_keys.json"

#define PLAY_PATTERNS_FILE              "/patterns.json"
#define CONFIG_PATTERNS_FILE            "/configPattern.json"

#define MAX_BACKUPS                     5

#define SERVER_NAME                     "https://api.openai.com/v1"

// Constants for easier Coding
#define ON true
#define OFF false
#define MICRO_SECONDS_PER_SECOND        1000000