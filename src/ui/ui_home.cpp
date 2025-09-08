#include "../app/services.h"
#include <lvgl.h>

static void open_wifi(lv_event_t *e) { AppServices::wifi_open(); }
static void open_ble(lv_event_t *e) { AppServices::ble_open(); }
static void open_rf(lv_event_t *e) { AppServices::rf_open(); }
static void open_rfid(lv_event_t *e) { AppServices::rfid_open(); }
static void open_ir(lv_event_t *e) { AppServices::ir_open(); }
static void open_scripts(lv_event_t *e) { AppServices::scripts_open(); }
static void open_files(lv_event_t *e) { AppServices::files_open(); }
static void open_settings(lv_event_t *e) { AppServices::settings_open(); }

void ui_build_home() {
    lv_obj_t *scr = lv_obj_create(NULL);
    lv_scr_load(scr);

    lv_obj_set_style_bg_color(scr, lv_color_black(), 0);

    lv_obj_t *title = lv_label_create(scr);
    lv_label_set_text(title, "Bruce");
    lv_obj_set_style_text_font(title, LV_FONT_DEFAULT, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 6);

    // Grid of 8 module buttons
    static const char *names[] = {"Wi-Fi", "BLE", "RF", "RFID", "IR", "Scripts", "Files", "Settings"};
    lv_event_cb_t cbs[] = {
        open_wifi, open_ble, open_rf, open_rfid, open_ir, open_scripts, open_files, open_settings
    };

    lv_obj_t *cont = lv_obj_create(scr);
    lv_obj_set_size(cont, 300, 180);
    lv_obj_align(cont, LV_ALIGN_CENTER, 0, 10);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_style_pad_gap(cont, 8, 0);
    lv_obj_set_style_bg_opa(cont, LV_OPA_0, 0);

    for (int i = 0; i < 8; i++) {
        lv_obj_t *btn = lv_btn_create(cont);
        lv_obj_set_size(btn, 140, 48);
        lv_obj_t *lbl = lv_label_create(btn);
        lv_label_set_text(lbl, names[i]);
        lv_obj_center(lbl);
        lv_obj_add_event_cb(btn, cbs[i], LV_EVENT_CLICKED, NULL);
    }

    // Footer
    lv_obj_t *status = lv_label_create(scr);
    lv_label_set_text(status, LV_SYMBOL_WIFI " Ready");
    lv_obj_align(status, LV_ALIGN_BOTTOM_LEFT, 6, -6);
}
