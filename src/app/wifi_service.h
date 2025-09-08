#pragma once
#include "common.h"
#include <functional>

namespace app::wifi {

struct Ap {
    std::string ssid;
    std::string bssid; // "AA:BB:CC:DD:EE:FF"
    int rssi;          // dBm
    uint8_t channel;   // 1..14
    bool secure;       // WPA/WPA2/WPA3?
};

struct Creds {
    std::string ssid;
    std::string password;
};

using ScanCb = std::function<void(const Ap &ap, bool done)>;

void init();
void tick();

Status scan_async(ScanCb cb); // streams results, then done=true
Status connect(const Creds &c);
Status disconnect();
bool is_connected();

Status deauth(const std::string &bssid, uint8_t channel, uint16_t seconds);
Status beacon_spam_start(const std::string &ssid, uint8_t channel);
Status beacon_spam_stop();

} // namespace app::wifi
