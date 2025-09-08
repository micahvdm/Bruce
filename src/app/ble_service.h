#pragma once
#include "common.h"
#include <functional>

namespace app::ble {

struct Dev {
    std::string name, addr;
    int rssi;
};

// Enum for the different spam types available in the ble_spam module
enum class SpamType {
    AppleJuice,
    SourApple,
    SwiftPair,
    Samsung,
    Android,
    TuttiFrutti,
};

void init();
void tick();

// --- Scanning ---
// WARNING: This is a BLOCKING call that shows a UI.
using ScanCb = std::function<void(const Dev &dev, bool done)>;
Status scan(ScanCb cb = nullptr); // Renamed to remove `_async` to reflect its blocking nature.
Status stop_scan();               // Not supported by the module.

// --- Spamming ---
// WARNING: This is a BLOCKING call that shows a UI.
Status spam_start(SpamType type);
Status spam_stop(); // Not supported by the module.

// --- BadUSB/HID over BLE ---
Status start_bad_usb();
Status start_media_controller();
Status start_ibeacon();
Status connect(const std::string &addr);
Status disconnect();
Status advertise_start(const std::string &name, const std::vector<uint8_t> &adv);
Status advertise_stop();

} // namespace app::ble
