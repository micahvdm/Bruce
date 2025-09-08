#pragma once
#include <Arduino.h>

// Core
#include "common.h"
#include "core/display.h"
#include "globals.h"

// JavaScript interpreter (no Lua)
#include "modules/bjs_interpreter/interpreter.h"

class ScriptsService {
public:
    // Runs a .js file from SD/LittleFS
    bool runFile(const String &path);

    // Runs raw JS code provided by the UI (quick pad)
    bool runCode(const String &code);

    // Opens the built-in picker loop (if provided by module)
    void openPicker();
};
