#include "SettingsManager.h"

SettingsManagerClass::SettingsManagerClass() {}

void SettingsManagerClass::begin(const String& filename) {
    _filename = filename;
    _synchronizeSettingsFromFile();
}

void SettingsManagerClass::addSetting(const String& key, const String& defaultValue) {
    // Add new setting only if it doesn't already exist
    if (_settings.find(key) == _settings.end()) {
        _settings[key] = defaultValue;
    }
}

void SettingsManagerClass::setSetting(const String& key, const String& value) {
    // Update setting only if key exists
    if (_settings.find(key) != _settings.end()) {
        _settings[key] = value;
    }
}

String SettingsManagerClass::getSetting(const String& key) {
    if (_settings.find(key) != _settings.end()) {
        return _settings[key];
    }
    return "";
}

void SettingsManagerClass::_synchronizeSettingsFromFile() {
    JsonDocument doc;

    if (LittleFsHelper::fileExists(_filename)) {
        String fileContent = LittleFsHelper::readFile(_filename);

        DeserializationError error = deserializeJson(doc, fileContent);
        if (error) {
            Serial.println("Failed to deserialize Settings file");
            return;
        }

        JsonObject obj = doc.as<JsonObject>();

        // Update only known settings from file
        for (JsonPair kv : obj) {
            String jsonKey = kv.key().c_str();
            String jsonValue = kv.value().as<String>();

            if (_settings.find(jsonKey) != _settings.end()) {
                _settings[jsonKey] = jsonValue;
            }
        }
    }

    saveSettings(); // Persist in case defaults were applied
}

void SettingsManagerClass::saveSettings() const {
    JsonDocument newDoc;

    for (const auto& setting : _settings) {
        newDoc[setting.first] = setting.second;
    }

    String content = "";
    serializeJson(newDoc, content);

    LittleFsHelper::writeFile(_filename, content);
}

// Global instance
SettingsManagerClass SettingsManager;
