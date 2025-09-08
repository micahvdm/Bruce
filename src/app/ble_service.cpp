#include "ble_service.h"

#if defined(__has_include)
#if __has_include("modules/ble/ble.h")
#include "modules/ble/ble.h"
#define HAVE_BRUCE_BLE 1
#endif
#if __has_include("modules/ble/ble_common.h")
#include "modules/ble/ble_common.h"
#endif
#if __has_include("modules/ble/ble_spam.h")
#include "modules/ble/ble_spam.h"
#endif
#if __has_include("modules/ble/ble_ninebot.h")
#include "modules/ble/ble_ninebot.h"
#endif
#endif

namespace app::ble {
static ScanCb g_cb;

// Mapping (change to your exports if they differ)
#ifdef HAVE_BRUCE_BLE
#ifndef MAP_BLE_SCAN_START
#define MAP_BLE_SCAN_START bruce_ble_scan_start
#endif
#ifndef MAP_BLE_SCAN_DONE
#define MAP_BLE_SCAN_DONE bruce_ble_scan_is_done
#endif
#ifndef MAP_BLE_SCAN_COUNT
#define MAP_BLE_SCAN_COUNT bruce_ble_scan_count
#endif
#ifndef MAP_BLE_SCAN_GET
#define MAP_BLE_SCAN_GET bruce_ble_scan_get // (idx,name*,len,addr*,len,&rssi)
#endif
#ifndef MAP_BLE_SCAN_STOP
#define MAP_BLE_SCAN_STOP bruce_ble_scan_stop
#endif
#ifndef MAP_BLE_CONNECT
#define MAP_BLE_CONNECT bruce_ble_connect
#endif
#ifndef MAP_BLE_DISCONNECT
#define MAP_BLE_DISCONNECT bruce_ble_disconnect
#endif
#ifndef MAP_BLE_ADV_START
#define MAP_BLE_ADV_START bruce_ble_adv_start // (name, adv, len)
#endif
#ifndef MAP_BLE_ADV_STOP
#define MAP_BLE_ADV_STOP bruce_ble_adv_stop
#endif
#endif

void init() {}
void tick() {}

Status scan_async(ScanCb cb) {
    if (!cb) return Status::InvalidArg;
#if defined(HAVE_BRUCE_BLE)
    if (!MAP_BLE_SCAN_START()) return Status::IOError;
    int last = 0;
    while (!MAP_BLE_SCAN_DONE()) {
        int n = MAP_BLE_SCAN_COUNT();
        for (int i = last; i < n; ++i) {
            char name[40]{0}, addr[20]{0};
            int rssi = 0;
            if (MAP_BLE_SCAN_GET(i, name, sizeof(name), addr, sizeof(addr), &rssi)) {
                cb(Dev{name, addr, rssi}, false);
            }
        }
        last = n;
#ifdef ARDUINO
        delay(50);
#endif
    }
    cb(Dev{}, true);
    return Status::Ok;
#else
    cb(Dev{"DemoBLE", "AA:BB:CC:DD:EE:FF", -55}, false);
    cb(Dev{}, true);
    return Status::Ok;
#endif
}
Status stop_scan() {
#if defined(HAVE_BRUCE_BLE)
    MAP_BLE_SCAN_STOP();
    return Status::Ok;
#else
    return Status::NotSupported;
#endif
}
Status connect(const std::string &addr) {
#if defined(HAVE_BRUCE_BLE)
    return MAP_BLE_CONNECT(addr.c_str()) ? Status::Ok : Status::IOError;
#else
    (void)addr;
    return Status::NotSupported;
#endif
}
Status disconnect() {
#if defined(HAVE_BRUCE_BLE)
    MAP_BLE_DISCONNECT();
    return Status::Ok;
#else
    return Status::NotSupported;
#endif
}
Status advertise_start(const std::string &name, const std::vector<uint8_t> &adv) {
#if defined(HAVE_BRUCE_BLE)
    return MAP_BLE_ADV_START(name.c_str(), adv.data(), adv.size()) ? Status::Ok : Status::IOError;
#else
    (void)name;
    (void)adv;
    return Status::NotSupported;
#endif
}
Status advertise_stop() {
#if defined(HAVE_BRUCE_BLE)
    MAP_BLE_ADV_STOP();
    return Status::Ok;
#else
    return Status::NotSupported;
#endif
}
} // namespace app::ble
