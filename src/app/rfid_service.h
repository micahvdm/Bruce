#pragma once
#include <Arduino.h>

// Core
#include "common.h"
#include "core/display.h"
#include "globals.h"

// Bruce RFID module (PN532/PN532Killer/125k)
#include "modules/rfid/PN532.h"
#include "modules/rfid/tag_o_matic.h"

class RFIDService {
public:
    // 13.56 MHz flows
    void readTag();
    void writeTag();
    void cloneTag();
    void writeNdef();
    void eraseTag();
    void saveToFile();
    void loadFromFile();
    void emulateTag(); // NDEF emulation (PN532 BLE)
    void amiibolink();
    void chameleon();

    // 125 kHz flows
    void read125k();

    // Config
    void setRfidModule(uint8_t moduleId); // maps to bruce.conf rfidModule
};
