#include "wifi_service.h"
#include <string.h>

// ── Modules/includes — only compiled if they exist ─────────────────────────────
#if defined(__has_include)
#if __has_include("modules/wifi/wifi.h")
#include "modules/wifi/wifi.h"
#define HAVE_BRUCE_WIFI 1
#endif
#if __has_include("modules/wifi/wifi_common.h")
#include "modules/wifi/wifi_common.h"
#endif
#if __has_include("modules/wifi/wifi_attacks.h")
#include "modules/wifi/wifi_attacks.h"
#endif
#if __has_include("modules/wifi/evil_portal.h")
#include "modules/wifi/evil_portal.h"
#endif
#if __has_include("modules/wifi/wardriving.h")
#include "modules/wifi/wardriving.h"
#endif
#if __has_include("modules/wifi/telnet.h")
#include "modules/wifi/telnet.h"
#endif
#if __has_include("modules/wifi/ssh.h")
#include "modules/wifi/ssh.h"
#endif
#if __has_include("modules/wifi/raw_sniffer.h")
#include "modules/wifi/raw_sniffer.h"
#endif
#if __has_include("modules/wifi/tcp_client.h")
#include "modules/wifi/tcp_client.h"
#endif
#if __has_include("modules/wifi/tcp_listener.h")
#include "modules/wifi/tcp_listener.h"
#endif
#if __has_include("modules/wifi/dpwo.h")
#include "modules/wifi/dpwo.h"
#endif
#if __has_include("modules/wifi/wireguard.h")
#include "modules/wifi/wireguard.h"
#endif
#if __has_include("modules/wifi/scan_hosts.h")
#include "modules/wifi/scan_hosts.h"
#endif
#if __has_include("modules/wifi/brucegotchi.h")
#include "modules/wifi/brucegotchi.h"
#endif
#endif

namespace app::wifi {

static ScanCb g_cb;

// ── Mapping lines (edit these to your real exports once) ───────────────────────
#ifdef HAVE_BRUCE_WIFI
// Scan lifecycle
#ifndef MAP_WIFI_SCAN_START
#define MAP_WIFI_SCAN_START bruce_wifi_scan_start
#endif
#ifndef MAP_WIFI_SCAN_DONE
#define MAP_WIFI_SCAN_DONE bruce_wifi_scan_is_done
#endif
#ifndef MAP_WIFI_SCAN_COUNT
#define MAP_WIFI_SCAN_COUNT bruce_wifi_scan_count
#endif
#ifndef MAP_WIFI_SCAN_GET
#define MAP_WIFI_SCAN_GET bruce_wifi_scan_get // (i, ssid*,ssid_len,bssid*,bssid_len,&rssi,&ch,&sec)
#endif

// Station control
#ifndef MAP_WIFI_STA_CONNECT
#define MAP_WIFI_STA_CONNECT bruce_wifi_sta_connect // (ssid, pass)
#endif
#ifndef MAP_WIFI_STA_DISCONNECT
#define MAP_WIFI_STA_DISCONNECT bruce_wifi_sta_disconnect
#endif
#ifndef MAP_WIFI_STA_IS_CONNECTED
#define MAP_WIFI_STA_IS_CONNECTED bruce_wifi_sta_is_connected
#endif

// Attacks
#ifndef MAP_WIFI_DEAUTH
#define MAP_WIFI_DEAUTH bruce_wifi_deauth // (bssid, ch, seconds)
#endif
#ifndef MAP_WIFI_BEACON_SPAM_START
#define MAP_WIFI_BEACON_SPAM_START bruce_wifi_beacon_spam_start // (ssid, ch)
#endif
#ifndef MAP_WIFI_BEACON_SPAM_STOP
#define MAP_WIFI_BEACON_SPAM_STOP bruce_wifi_beacon_spam_stop
#endif
#endif
// ───────────────────────────────────────────────────────────────────────────────

void init() {}
void tick() {}

Status scan_async(ScanCb cb) {
    if (!cb) return Status::InvalidArg;
    g_cb = cb;

#ifdef HAVE_BRUCE_WIFI
    if (!MAP_WIFI_SCAN_START()) return Status::IOError;

    uint32_t last_emit = now_ms();
    int last_count = 0;

    while (!MAP_WIFI_SCAN_DONE()) {
        if (now_ms() - last_emit > 200) {
            int n = MAP_WIFI_SCAN_COUNT();
            for (int i = last_count; i < n; ++i) {
                char ssid[64]{0}, bssid[20]{0};
                int rssi = 0;
                uint8_t ch = 0;
                bool sec = false;
                if (MAP_WIFI_SCAN_GET(i, ssid, sizeof(ssid), bssid, sizeof(bssid), &rssi, &ch, &sec)) {
                    cb(Ap{ssid, bssid, rssi, ch, sec}, false);
                }
            }
            last_count = n;
            last_emit = now_ms();
        }
#ifdef ARDUINO
        delay(10);
#endif
    }

    // flush the rest
    int n = MAP_WIFI_SCAN_COUNT();
    for (int i = last_count; i < n; ++i) {
        char ssid[64]{0}, bssid[20]{0};
        int rssi = 0;
        uint8_t ch = 0;
        bool sec = false;
        if (MAP_WIFI_SCAN_GET(i, ssid, sizeof(ssid), bssid, sizeof(bssid), &rssi, &ch, &sec)) {
            cb(Ap{ssid, bssid, rssi, ch, sec}, false);
        }
    }

    cb(Ap{}, true);
    return Status::Ok;
#else
    // Mock so UI is usable before wiring
    cb(Ap{"DemoAP", "00:11:22:33:44:55", -42, 6, true}, false);
    cb(Ap{"OpenNet", "66:55:44:33:22:11", -68, 1, false}, false);
    cb(Ap{}, true);
    return Status::Ok;
#endif
}

Status connect(const Creds &c) {
#ifdef HAVE_BRUCE_WIFI
    return MAP_WIFI_STA_CONNECT(c.ssid.c_str(), c.password.c_str()) ? Status::Ok : Status::IOError;
#else
    (void)c;
    return Status::NotSupported;
#endif
}
Status disconnect() {
#ifdef HAVE_BRUCE_WIFI
    MAP_WIFI_STA_DISCONNECT();
    return Status::Ok;
#else
    return Status::NotSupported;
#endif
}
bool is_connected() {
#ifdef HAVE_BRUCE_WIFI
    return MAP_WIFI_STA_IS_CONNECTED();
#else
    return false;
#endif
}

Status deauth(const std::string &bssid, uint8_t ch, uint16_t seconds) {
#ifdef HAVE_BRUCE_WIFI
    return MAP_WIFI_DEAUTH(bssid.c_str(), ch, seconds) ? Status::Ok : Status::Denied;
#else
    (void)bssid;
    (void)ch;
    (void)seconds;
    return Status::NotSupported;
#endif
}
Status beacon_spam_start(const std::string &ssid, uint8_t ch) {
#ifdef HAVE_BRUCE_WIFI
    return MAP_WIFI_BEACON_SPAM_START(ssid.c_str(), ch) ? Status::Ok : Status::IOError;
#else
    (void)ssid;
    (void)ch;
    return Status::NotSupported;
#endif
}
Status beacon_spam_stop() {
#ifdef HAVE_BRUCE_WIFI
    MAP_WIFI_BEACON_SPAM_STOP();
    return Status::Ok;
#else
    return Status::NotSupported;
#endif
}

} // namespace app::wifi
