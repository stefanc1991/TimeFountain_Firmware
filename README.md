
# 💧 AI-Supported Time Fountain

A smart, visually striking indoor fountain using programmable LEDs and vibrating magnets to simulate stunning visual effects like water flowing upwards or hovering in mid-air. Fully programmable through a natural-language interface powered by GPT and controlled by an ESP32 running RTOS tasks.

![Lichtmuster GIF 2](FountainDemo2.gif)

*Fig.1: Current version showing default pattern.*

![Lichtmuster GIF](FountainDemo.gif)

*Fig.2: Prototype showing various light patterns.*

> ⚠️ Note: Flickering is caused by the camera's frame rate. The live experience is much smoother.
---

## 🧠 Overview

This DIY project combines real-time microcontroller programming, web technologies, and AI integration to:

* Creates time-frozen and time-reversed water effects using stroboscopic LED control
* Uses a pump and electromagnet to control water flow
* Allows users to generate and edit light patterns using natural language
* Provides OTA updates, web UI, and IR remote control

---

## ⚙️ Hardware Requirements

* ESP32 development board (tested on `nodemcu-32s`)
* APA102c or compatible addressable LEDs
* Electromagnet for water modulation
* IR receiver and remote
* Pump + silicone tubes
* Power supply: USB C
* 3D-printed fountain structure

---

## 🔧 PlatformIO Configuration

The environment is defined in `platformio.ini`:

```ini
[env:nodemcu-32s]
platform = espressif32
board = nodemcu-32s
framework = arduino
board_build.filesystem = littlefs
monitor_speed = 115200
upload_speed = 1000000

lib_deps = 
  bblanchon/ArduinoJson@^7.2.0
  fastled/FastLED@^3.7.8
  z3t0/IRremote@^4.4.1
  esphome/AsyncTCP-esphome@2.1.4
  esphome/ESPAsyncWebServer-esphome@^3.2.2
```

---

## 🧩 Key Components

### 1. Real-Time System

* `main.cpp` orchestrates the setup, loop and additional tasks for light and magnet control.
* Tasks run on ESP32 using FreeRTOS:

  * `lightEventSchedularTask`: Prepares LED color changes.
  * `lightControlTask`: Triggers LED changes using SPI.
  * `magnetControlTask`: Toggles magnet polarity.

### 2. Pattern Architecture

* Patterns are stored as JSON (in LittleFS):

  * `/patterns.json`: Used in play mode
  * `/configPattern.json`: Used in config mode

* Each pattern consists of multiple `WaterStreams`, each defined by:
  * `DC` – Duty Cycle
  * `IPS` – Initial Phase Shift
  * `TCP`, `BCP` – Top and Bottom Color Phases
  * `MP`, `VP` – Motion and Visibility phases

### 3. AI Integration

* The AI assistant uses the GPT API.
* Users can create, modify, delete, or change the order of light patterns via natural language (in the web interface).
* GPT system prompt and tools are described in `assistant_definitions.cpp`.

#### Tools Provided to the Assistant:

| Function                             | Description           |
| ------------------------------------ | --------------------- |
| `getSelectedPattern()`               | Fetch active pattern  |
| `createPattern(patternJson)`         | Create new pattern    |
| `modifySelectedPattern(patternJson)` | Modify active pattern |
| `deleteSelectedPattern()`            | Remove active pattern |
| `moveSelectedPattern(steps)`         | Move pattern up/down  |
| `undo()`                             | Rollback last change  |

> ⚠️ Note: The assistant uses abbreviations in pattern JSON (like `WS`, `IPS`, etc.) and will **not** display raw JSON to users. Nevertheless, users can download the JSON files and modify them manually.

---

## 🌐 Web & OTA Features

* Fully asynchronous web interface (based on ESPAsyncWebServer)
* Routes:

  * `/`: Home page
  * `/ai`: AI interaction and editing
  * `/setup`: OTA and device setup
* Web UI allows drag-and-drop firmware updates
* mDNS (via `timefountain.local`) is supported for easy access

---

## 🔢 IR Remote Integration

Default IR key mappings:

| Key   | IR Code |
| ----- | ------- |
| Play  | `0x5E`  |
| Menu  | `0x02`  |
| Up    | `0x0B`  |
| Down  | `0x0D`  |
| Right | `0x07`  |
| Left  | `0x08`  |
| OK    | `0x5D`  |

You can customize keys via /setup Web UI.

---

## 🙏 Credits

* **Stefan Conrad** – Creator & developer
* **FastLED**, **ESPAsyncWebServer**, **ArduinoJson** – Open-source libraries
* **OpenAI GPT-4o** – AI pattern creation

---