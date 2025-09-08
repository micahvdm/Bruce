#include "settings_service.h"

#if defined(__has_include)
// Umbrella include for Settings menu (colors/brightness etc.)
#if __has_include("core/menu_items/ConfigMenu.h")
#include "core/menu_items/ConfigMenu.h"
#endif

// Core config/globals (persist to bruce.conf)
#if __has_include("core/config.h")
#include "core/config.h"
#define HAVE_BRUCE_CONFIG 1
#endif
#if __has_include("include/globals.h")
#include "include/globals.h"
#elif __has_include("globals.h")
#include "globals.h"
#endif
#endif

namespace app::settings {

// Map to concrete config setters used by Config menu  //
#ifdef HAVE_BRUCE_CONFIG
#ifndef MAP_SET_BRIGHT
#define MAP_SET_BRIGHT setBright
#endif
#ifndef MAP_SET_ROT
#define MAP_SET_ROT setRotation
#endif
#ifndef MAP_SET_TZ
#define MAP_SET_TZ setTimezone
#endif
#ifndef MAP_SET_SOUND
#define MAP_SET_SOUND setSoundEnabled
#endif

#ifndef MAP_SET_COLOR_PRIMARY
#define MAP_SET_COLOR_PRIMARY setPrimaryColor
#endif
#ifndef MAP_SET_COLOR_SECONDARY
#define MAP_SET_COLOR_SECONDARY setSecondaryColor
#endif
#ifndef MAP_SET_COLOR_BG
#define MAP_SET_COLOR_BG setBgColor
#endif

// RF & RFID
#ifndef MAP_SET_RF_PINS
#define MAP_SET_RF_PINS setRfPins
#endif
#ifndef MAP_SET_RF_FREQ
#define MAP_SET_RF_FREQ setRfFreq
#endif
#ifndef MAP_SET_RF_FIXED
#define MAP_SET_RF_FIXED setRfFxdFreq
#endif
#ifndef MAP_SET_RF_RANGE
#define MAP_SET_RF_RANGE setRfScanRange
#endif
#ifndef MAP_SET_RF_MODULE
#define MAP_SET_RF_MODULE setRfModule
#endif
#ifndef MAP_SET_RFID_MODULE
#define MAP_SET_RFID_MODULE setRfidModule
#endif

// Wi-Fi boot
#ifndef MAP_SET_WIFI_AT_STARTUP
#define MAP_SET_WIFI_AT_STARTUP setWifiAtStartup
#endif

// Persist
#ifndef MAP_SAVE_CONF
#define MAP_SAVE_CONF saveConfigFile
#endif
#ifndef MAP_FACTORY_RESET
#define MAP_FACTORY_RESET factory_reset
#endif
#endif

void init() {}
void tick() {}

void setBrightness(uint8_t pct) {
#ifdef HAVE_BRUCE_CONFIG MAP_SET_BRIGHT(pct);
#endif
}
void setRotation(uint8_t rot) {
#ifdef HAVE_BRUCE_CONFIG MAP_SET_ROT(rot);
#endif
}
void setTimezone(int8_t tz) {
#ifdef HAVE_BRUCE_CONFIG MAP_SET_TZ(tz);
#endif
}
void setSoundEnabled(bool on) {
#ifdef HAVE_BRUCE_CONFIG MAP_SET_SOUND(on ? 1 : 0);
#endif
}

void setUiColors(uint16_t primary, uint16_t secondary, uint16_t bg) {
#ifdef HAVE_BRUCE_CONFIG
    MAP_SET_COLOR_PRIMARY(primary);
    MAP_SET_COLOR_SECONDARY(secondary);
    MAP_SET_COLOR_BG(bg);
#endif
}

void setRfPins(int tx, int rx) {
#ifdef HAVE_BRUCE_CONFIG MAP_SET_RF_PINS(tx, rx);
#endif
}
void setRfFreq(float mhz) {
#ifdef HAVE_BRUCE_CONFIG MAP_SET_RF_FREQ(mhz);
#endif
}
void setRfFixedFreq(bool fixed) {
#ifdef HAVE_BRUCE_CONFIG MAP_SET_RF_FIXED(fixed ? 1 : 0);

#endif
}
void setRfScanRange(uint8_t range) {
#ifdef HAVE_BRUCE_CONFIG MAP_SET_RF_RANGE(range);
#endif
}
void setRfModule(uint8_t id) {
#ifdef HAVE_BRUCE_CONFIG MAP_SET_RF_MODULE(id);
#endif
}
void setRfidModule(uint8_t id) {
#ifdef HAVE_BRUCE_CONFIG MAP_SET_RFID_MODULE(id);
#endif
}

void setWifiAtStartup(bool on) {
#ifdef HAVE_BRUCE_CONFIG MAP_SET_WIFI_AT_STARTUP(on ? 1 : 0);
#endif
}

bool save() {
#ifdef HAVE_BRUCE_CONFIG return MAP_SAVE_CONF();
#else return false;
#endif
}
void factoryReset() {
#ifdef HAVE_BRUCE_CONFIG MAP_FACTORY_RESET();
#endif
}

} // namespace app::settings
