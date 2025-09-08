#pragma once
#include <Arduino.h>

// Core & UI
#include "common.h"
#include "core/display.h"
#include "globals.h"

// Bruce Files module
// #include "modules/files/files.h"

enum class FilesFS : uint8_t { Auto = 0, SD = 1, LittleFS = 2 };

class FilesService {
public:
    // Managers open the module UIs (SD/LittleFS)
    void openSdManager();
    void openLittleFsManager();

    // Common helpers used by touch UI:
    bool readText(const String &path, String &out);
    bool readBinary(const String &path, std::vector<uint8_t> &out);
    bool writeText(const String &path, const String &data, FilesFS fs = FilesFS::Auto);
    bool copy(const String &from, const String &to);
    bool remove(const String &path);
    bool mkdirs(const String &path);
    bool rename(const String &from, const String &to);

    // Quick actions
    void viewFile(const String &path);
    void txRfFromSub(const String &path); // convenience: send *.sub via RF module
};
