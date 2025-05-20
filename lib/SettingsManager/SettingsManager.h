#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include <map>
#include "LittleFsHelper.h"

class SettingsManagerClass {
public:
    SettingsManagerClass();
    void begin(const String& filename);
    void addSetting(const String& key, const String& defaultValue);
    void setSetting(const String& key, const String& value);
    String getSetting(const String& key);
    void removeSetting(const String& key);
    void saveSettings() const;

private:
    String _filename;
    void _synchronizeSettingsFromFile();

    std::map<String, String> _settings;
};

extern SettingsManagerClass SettingsManager;
