#pragma once
#include <Arduino.h>

// Core config (persists to bruce.conf)
#include "common.h"
#include "core/config.h"
#include "globals.h"

struct UiColors {
    uint16_t primary; // RGB565
    uint16_t secondary;
    uint16_t background;
};

class SettingsService {
public:
    // Brightness / display
    void setBrightness(uint8_t pct);
    void setRotation(uint8_t rot);
    void setTimezone(int8_t tz);

    // Colors
    void setUiColors(const UiColors &c);

    // Audio / misc
    void setSoundEnabled(bool on);

    // RF / RFID shortcuts (same fields as bruce.conf)
    void setRfPins(int tx, int rx);
    void setRfFreq(float mhz);
    void setRfFixedFreq(bool fixed);
    void setRfScanRange(uint8_t range);
    void setRfModule(uint8_t moduleId);
    void setRfidModule(uint8_t moduleId);

    // WiFi boot
    void setWifiAtStartup(bool on);

    // Persistence
    bool save();
    void factoryReset();
};
