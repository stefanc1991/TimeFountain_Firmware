#pragma once

#include <Arduino.h>

namespace LittleFsHelper {

    bool begin();
    bool writeFile(const String path, const String content);
    String readFile(const String path);
    bool fileExists(const String path);
    bool deleteFile(const String path);
    String listFilesInFolder(const String folderPath);
    bool appendRawDataToFile(const String path, const uint8_t* data, size_t size);
}
