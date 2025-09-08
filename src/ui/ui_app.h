#pragma once
#include <Arduino.h>
#include <lvgl.h>

namespace ui {

class UiApp {
public:
    bool begin(); // init display, touch, LVGL, build screens
    void loop();  // call in main loop

private:
    void build_root(); // tabs, bars, etc.
    void build_wifi_tab(lv_obj_t *tab);
    void build_ble_tab(lv_obj_t *tab);
    void build_rf_tab(lv_obj_t *tab);
    void build_rfid_tab(lv_obj_t *tab);
    void build_ir_tab(lv_obj_t *tab);
    void build_files_tab(lv_obj_t *tab);
    void build_scripts_tab(lv_obj_t *tab);
    void build_settings_tab(lv_obj_t *tab);

    // wifi helpers
    void wifi_start_scan();
    void wifi_add_row(const char *ssid, const char *bssid, int rssi, uint8_t ch, bool sec);

private:
    // UI handles we keep
    lv_obj_t *tabs_ = nullptr;
    lv_obj_t *wifi_list_ = nullptr;
    lv_obj_t *wifi_spinner_ = nullptr;
    lv_obj_t *wifi_status_ = nullptr;

    // periodic tick for LVGL
    uint32_t last_lv_tick_ms_ = 0;
};

} // namespace ui
