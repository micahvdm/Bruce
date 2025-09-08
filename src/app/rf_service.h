#pragma once
#include <Arduino.h>

// Core & UI glue
#include "common.h"
#include "core/display.h"
#include "core/settings.h"
#include "core/utils.h"
#include "globals.h"

// Bruce RF module
#include "modules/rf/record.h"
#include "modules/rf/rf_bruteforce.h"
#include "modules/rf/rf_jammer.h"
#include "modules/rf/rf_listen.h"
#include "modules/rf/rf_scan.h"
#include "modules/rf/rf_send.h"
#include "modules/rf/rf_spectrum.h"
#include "modules/rf/rf_waterfall.h"

class RFService {
public:
    // High-level actions to map to UI buttons / screens
    void openScanCopy();  // scan + copy flow
    void openRecordRaw(); // RMT raw recorder UI
    void replayLast();    // replay last captured
    void openSpectrum();  // waveform/spectrum UI
    void openSquarewaveSpectrum();
    void jammerFull();
    void jammerIntermittent();

    // Transmit helpers (for file pickers, quick-send, etc)
    bool txFromSubFile(const String &path); // *.sub
    bool txCustom(const String &hexKey, uint32_t hz, int te, int count);

    // Config hooks (normally exposed in SettingsService, but provided for convenience)
    void setRfPins(int tx, int rx);
    void setRfModule(uint8_t moduleId); // matches bruce.conf rfModule
    void setRfFrequency(float mhz);
};
