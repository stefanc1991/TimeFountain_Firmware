
#include "LittleFsHelper.h"

#include <ArduinoJson.h>
#include <LittleFS.h>

namespace LittleFsHelper {

    bool begin() {          // synchro mit magnet bauen
        return LittleFS.begin();
    }

    bool writeFile(const String path, const String content) {
        File file = LittleFS.open(path, "w", true);
        if (file) {
            file.print(content);
            file.close();
            return true;
        }
        return false;
    }

    String readFile(const String path) {
        String content = "";
        File file = LittleFS.open(path, "r");
        if (file) {
            while (file.available()) {
                content += char(file.read());
            }
            file.close();
        }
        return content;
    }

    String listFilesInFolder(const String folderPath) {
        JsonDocument jsonDocument;

        File dir = LittleFS.open(folderPath);
        File file = dir.openNextFile();
        
        while (file) {
            if (!file.isDirectory()) {
                JsonObject fileInfo = jsonDocument.to<JsonObject>();
                fileInfo["name"] = String(file.name());
                fileInfo["size"] = String(file.size());
                Serial.println(fileInfo["name"].as<String>());
            }
            file = dir.openNextFile();
        }
        
        dir.close();

        String jsonString;
        serializeJson(jsonDocument, jsonString);
        return jsonString;
    }

    bool fileExists(const String path) {
        return LittleFS.exists(path);
    }

    bool deleteFile(const String path) {
        return LittleFS.remove(path);
    }
    
    bool appendRawDataToFile(const String path, const uint8_t* data, size_t size) {
        File file = LittleFS.open(path, "a");
        if (file) {
            size_t bytesWritten = file.write(data, size);
            file.close();
            return (bytesWritten == size);
        }
        return false;
    }
}
