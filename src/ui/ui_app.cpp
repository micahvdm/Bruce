#include "ui_app.h"

// ===================== DISPLAY/TSC CONFIG (edit pins here) =====================
// ILI9341 SPI on ESP32-S3 (example pins; change to your wiring)
#ifndef LCD_PIN_MOSI
#define LCD_PIN_MOSI 11
#endif
#ifndef LCD_PIN_MISO
#define LCD_PIN_MISO 13
#endif
#ifndef LCD_PIN_SCLK
#define LCD_PIN_SCLK 12
#endif
#ifndef LCD_PIN_CS
#define LCD_PIN_CS 10
#endif
#ifndef LCD_PIN_DC
#define LCD_PIN_DC 21
#endif
#ifndef LCD_PIN_RST
#define LCD_PIN_RST 18
#endif
#ifndef LCD_PIN_BL
#define LCD_PIN_BL 4
#endif

// Touch (XPT2046 on SPI) — comment out if you use I2C FT6236 instead
#define USE_XPT2046 1
#if USE_XPT2046
#ifndef TCH_PIN_CS
#define TCH_PIN_CS 9
#endif
#ifndef TCH_PIN_IRQ
#define TCH_PIN_IRQ 7
#endif
#endif

// ===================== LovyanGFX (ILI9341) setup =====================
#define LGFX_USE_V1
#include <LovyanGFX.hpp>

class LGFX : public lgfx::LGFX_Device {
    lgfx::Panel_ILI9341 _panel;
    lgfx::Bus_SPI _bus;
#if USE_XPT2046
    lgfx::Touch_XPT2046 _touch;
#endif
public:
    LGFX() {
        { // SPI bus
            auto cfg = _bus.config();
            cfg.spi_host = SPI3_HOST;
            cfg.spi_mode = 0;
            cfg.freq_write = 40000000;
            cfg.freq_read = 16000000;
            cfg.spi_3wire = false;
            cfg.use_lock = true;
            cfg.dma_channel = 1;
            cfg.pin_sclk = LCD_PIN_SCLK;
            cfg.pin_mosi = LCD_PIN_MOSI;
            cfg.pin_miso = LCD_PIN_MISO;
            cfg.pin_dc = LCD_PIN_DC;
            _bus.config(cfg);
            _panel.setBus(&_bus);
        }
        { // Panel
            auto cfg = _panel.config();
            cfg.pin_cs = LCD_PIN_CS;
            cfg.pin_rst = LCD_PIN_RST;
            cfg.pin_busy = -1;
            cfg.memory_width = 240;
            cfg.memory_height = 320;
            cfg.panel_width = 240;
            cfg.panel_height = 320;
            cfg.offset_x = 0;
            cfg.offset_y = 0;
            cfg.invert = true;
            cfg.rgb_order = false;
            cfg.dlen_16bit = false;
            cfg.bus_shared = true;
            _panel.config(cfg);
        }
#if USE_XPT2046
        { // Touch
            auto cfg = _touch.config();
            cfg.x_min = 200;
            cfg.x_max = 3800;
            cfg.y_min = 200;
            cfg.y_max = 3800;
            cfg.pin_int = TCH_PIN_IRQ;
            cfg.bus_shared = true;
            cfg.offset_rotation = 0;
            cfg.spi_host = SPI3_HOST;
            cfg.freq = 1000000;
            cfg.pin_sclk = LCD_PIN_SCLK;
            cfg.pin_mosi = LCD_PIN_MOSI;
            cfg.pin_miso = LCD_PIN_MISO;
            cfg.pin_cs = TCH_PIN_CS;
            _touch.config(cfg);
            _panel.setTouch(&_touch);
        }
#endif
        setPanel(&_panel);
    }
};

static LGFX gfx;

// ===================== LVGL driver glue =====================
static void lvgl_flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *pxmap) {
    // pxmap: RGB565
    int32_t w = (area->x2 - area->x1 + 1);
    int32_t h = (area->y2 - area->y1 + 1);
    gfx.startWrite();
    gfx.setAddrWindow(area->x1, area->y1, w, h);
    gfx.pushPixels((lgfx::rgb565_t *)pxmap, w * h);
    gfx.endWrite();
    lv_disp_flush_ready(disp);
}

#if USE_XPT2046
static bool lvgl_touch_read_cb(lv_indev_t *indev, lv_indev_data_t *data) {
    uint16_t x, y;
    bool touched = gfx.getTouch(&x, &y);
    if (touched) {
        data->state = LV_INDEV_STATE_PRESSED;
        data->point.x = x;
        data->point.y = y;
    } else {
        data->state = LV_INDEV_STATE_RELEASED;
    }
    return false;
}
#endif

// ===================== App Services =====================
#include "app/services.h"
using namespace app;

namespace ui {

bool UiApp::begin() {
    // HW
    gfx.begin();
    if (LCD_PIN_BL >= 0) {
        pinMode(LCD_PIN_BL, OUTPUT);
        digitalWrite(LCD_PIN_BL, HIGH);
    }
    gfx.setBrightness(255);

    // LVGL
    lv_init();

    // Display
    static lv_color_t buf1[240 * 30]; // 30 rows (tune for PSRAM)
    static lv_color_t buf2[240 * 30];
    lv_display_t *disp = lv_display_create(240, 320);
    lv_display_set_buffers(disp, buf1, buf2, sizeof(buf1), LV_DISPLAY_RENDER_MODE_PARTIAL);
    lv_display_set_flush_cb(disp, lvgl_flush_cb);

#if USE_XPT2046
    lv_indev_t *indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev, lvgl_touch_read_cb);
#endif

    // Services
    services_init();

    // UI tree
    build_root();
    return true;
}

void UiApp::loop() {
    // LVGL tick
    uint32_t now = millis();
    uint32_t delta = (last_lv_tick_ms_ == 0) ? 1 : (now - last_lv_tick_ms_);
    last_lv_tick_ms_ = now;
    lv_tick_inc(delta);
    lv_timer_handler(); // pump UI

    services_tick();
}

// ===================== UI Builders =====================

static lv_obj_t *make_btn(lv_obj_t *parent, const char *txt, lv_event_cb_t cb) {
    lv_obj_t *b = lv_button_create(parent);
    lv_obj_t *l = lv_label_create(b);
    lv_label_set_text(l, txt);
    lv_obj_center(l);
    if (cb) lv_obj_add_event_cb(b, cb, LV_EVENT_CLICKED, nullptr);
    return b;
}

void UiApp::build_root() {
    tabs_ = lv_tabview_create(lv_screen_active(), LV_DIR_TOP, 36);
    lv_obj_set_size(tabs_, 240, 320);

    // Create tabs
    lv_obj_t *tab_wifi = lv_tabview_add_tab(tabs_, "WiFi");
    lv_obj_t *tab_ble = lv_tabview_add_tab(tabs_, "BLE");
    lv_obj_t *tab_rf = lv_tabview_add_tab(tabs_, "RF");
    lv_obj_t *tab_rfid = lv_tabview_add_tab(tabs_, "RFID");
    lv_obj_t *tab_ir = lv_tabview_add_tab(tabs_, "IR");
    lv_obj_t *tab_files = lv_tabview_add_tab(tabs_, "Files");
    lv_obj_t *tab_scripts = lv_tabview_add_tab(tabs_, "Scripts");
    lv_obj_t *tab_set = lv_tabview_add_tab(tabs_, "Settings");

    build_wifi_tab(tab_wifi);
    build_ble_tab(tab_ble);
    build_rf_tab(tab_rf);
    build_rfid_tab(tab_rfid);
    build_ir_tab(tab_ir);
    build_files_tab(tab_files);
    build_scripts_tab(tab_scripts);
    build_settings_tab(tab_set);
}

// ---------- WiFi ----------
static void on_wifi_scan(lv_event_t *e) {
    reinterpret_cast<UiApp *>(lv_event_get_user_data(e))->wifi_start_scan();
}
void UiApp::build_wifi_tab(lv_obj_t *tab) {
    lv_obj_set_flex_flow(tab, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_all(tab, 6, 0);

    lv_obj_t *row = lv_obj_create(tab);
    lv_obj_set_size(row, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_all(row, 4, 0);
    lv_obj_set_style_pad_column(row, 4, 0);
    lv_obj_clear_flag(row, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *btnScan = make_btn(row, "Scan", [](lv_event_t *e) {
        UiApp *self = static_cast<UiApp *>(lv_event_get_user_data(e));
        self->wifi_start_scan();
    });
    lv_obj_add_event_user_data(btnScan, this);

    wifi_spinner_ = lv_spinner_create(row, 1000, 60);
    lv_obj_add_flag(wifi_spinner_, LV_OBJ_FLAG_HIDDEN);

    wifi_status_ = lv_label_create(row);
    lv_label_set_text(wifi_status_, "");

    wifi_list_ = lv_list_create(tab);
    lv_obj_set_size(wifi_list_, LV_PCT(100), LV_PCT(100));
}

void UiApp::wifi_start_scan() {
    lv_obj_clear_flag(wifi_spinner_, LV_OBJ_FLAG_HIDDEN);
    lv_list_clear(wifi_list_);
    lv_label_set_text(wifi_status_, "Scanning…");

    wifi::scan_async([this](const wifi::Ap &ap, bool done) {
        if (!done) {
            wifi_add_row(ap.ssid.c_str(), ap.bssid.c_str(), ap.rssi, ap.channel, ap.secure);
            return;
        }
        lv_obj_add_flag(wifi_spinner_, LV_OBJ_FLAG_HIDDEN);
        lv_label_set_text(wifi_status_, "Done");
    });
}

void UiApp::wifi_add_row(const char *ssid, const char *bssid, int rssi, uint8_t ch, bool sec) {
    char line[128];
    snprintf(line, sizeof(line), "%s  (%s)  %ddBm  ch%u %s", ssid, bssid, rssi, ch, sec ? "🔒" : "open");
    lv_obj_t *item = lv_list_add_text(wifi_list_, line);
    (void)item;
}

// ---------- BLE ----------
void UiApp::build_ble_tab(lv_obj_t *tab) {
    lv_obj_set_flex_flow(tab, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_all(tab, 6, 0);

    lv_obj_t *row = lv_obj_create(tab);
    lv_obj_set_size(row, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
    lv_obj_clear_flag(row, LV_OBJ_FLAG_SCROLLABLE);

    make_btn(row, "Scan", [](lv_event_t *) {
        ble::scan_async([](const ble::Dev &d, bool done) {
            // You can add to a list like WiFi; keeping minimal here
            (void)d;
            (void)done;
        });
    });
    make_btn(row, "Stop", [](lv_event_t *) { ble::stop_scan(); });
}

// ---------- RF ----------
void UiApp::build_rf_tab(lv_obj_t *tab) {
    lv_obj_set_flex_flow(tab, LV_FLEX_FLOW_WRAP);
    lv_obj_set_style_pad_all(tab, 6, 0);

    make_btn(tab, "Scan/Copy", [](lv_event_t *) { rf::openScanCopy(); });
    make_btn(tab, "Record Raw", [](lv_event_t *) { rf::openRecordRaw(); });
    make_btn(tab, "Replay", [](lv_event_t *) { rf::replayLast(); });
    make_btn(tab, "Spectrum", [](lv_event_t *) { rf::openSpectrum(); });
    make_btn(tab, "Squarewave", [](lv_event_t *) { rf::openSquarewaveSpectrum(); });
    make_btn(tab, "Jammer Full", [](lv_event_t *) { rf::jammerFull(); });
    make_btn(tab, "Jammer Int", [](lv_event_t *) { rf::jammerIntermittent(); });
}

// ---------- RFID ----------
void UiApp::build_rfid_tab(lv_obj_t *tab) {
    lv_obj_set_flex_flow(tab, LV_FLEX_FLOW_WRAP);
    lv_obj_set_style_pad_all(tab, 6, 0);

    make_btn(tab, "Read", [](lv_event_t *) { rfid::readTag(); });
    make_btn(tab, "Write", [](lv_event_t *) { rfid::writeTag(); });
    make_btn(tab, "Clone", [](lv_event_t *) { rfid::cloneTag(); });
    make_btn(tab, "Write NDEF", [](lv_event_t *) { rfid::writeNdef(); });
    make_btn(tab, "Erase", [](lv_event_t *) { rfid::eraseTag(); });
    make_btn(tab, "Save File", [](lv_event_t *) { rfid::saveToFile(); });
    make_btn(tab, "Load File", [](lv_event_t *) { rfid::loadFromFile(); });
    make_btn(tab, "Emulate", [](lv_event_t *) { rfid::emulateTag(); });
    make_btn(tab, "Amiibolink", [](lv_event_t *) { rfid::amiibolink(); });
    make_btn(tab, "Chameleon", [](lv_event_t *) { rfid::chameleon(); });
    make_btn(tab, "Read 125k", [](lv_event_t *) { rfid::read125k(); });
}

// ---------- IR ----------
void UiApp::build_ir_tab(lv_obj_t *tab) {
    lv_obj_set_flex_flow(tab, LV_FLEX_FLOW_WRAP);
    lv_obj_set_style_pad_all(tab, 6, 0);

    make_btn(tab, "Learn", [](lv_event_t *) {
        ir::learn_async(
            [](const ir::Signal &sig, bool done) {
                (void)sig;
                (void)done; // You can show protocol, length, etc.
            },
            5000
        );
    });
    make_btn(tab, "Send last", [](lv_event_t *) {
        // Example store last in your app state; kept minimal here
    });
}

// ---------- Files ----------
void UiApp::build_files_tab(lv_obj_t *tab) {
    lv_obj_set_flex_flow(tab, LV_FLEX_FLOW_WRAP);
    lv_obj_set_style_pad_all(tab, 6, 0);
    make_btn(tab, "SD Manager", [](lv_event_t *) { files::openSdManager(); });
    make_btn(tab, "LittleFS Mgr", [](lv_event_t *) { files::openLittleFsManager(); });
}

// ---------- Scripts (JS) ----------
void UiApp::build_scripts_tab(lv_obj_t *tab) {
    lv_obj_set_flex_flow(tab, LV_FLEX_FLOW_WRAP);
    lv_obj_set_style_pad_all(tab, 6, 0);
    make_btn(tab, "Open Picker", [](lv_event_t *) { scripts::openPicker(); });
    make_btn(tab, "Run demo.js", [](lv_event_t *) { scripts::runFile("/scripts/demo.js"); });
}

// ---------- Settings ----------
void UiApp::build_settings_tab(lv_obj_t *tab) {
    lv_obj_set_flex_flow(tab, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_all(tab, 6, 0);

    // Brightness slider
    lv_obj_t *lbl = lv_label_create(tab);
    lv_label_set_text(lbl, "Brightness");
    lv_obj_t *sld = lv_slider_create(tab);
    lv_slider_set_range(sld, 1, 100);
    lv_slider_set_value(sld, 80, LV_ANIM_OFF);
    lv_obj_add_event_cb(
        sld,
        [](lv_event_t *e) {
            int v = lv_slider_get_value(lv_event_get_target(e));
            settings::setBrightness((uint8_t)v);
        },
        LV_EVENT_VALUE_CHANGED,
        nullptr
    );

    // Rotation dropdown
    lv_obj_t *dd = lv_dropdown_create(tab);
    lv_dropdown_set_options(dd, "0\n1\n2\n3");
    lv_obj_add_event_cb(
        dd,
        [](lv_event_t *e) {
            char buf[8];
            lv_dropdown_get_selected_str(lv_event_get_target(e), buf, sizeof(buf));
            settings::setRotation((uint8_t)atoi(buf));
        },
        LV_EVENT_VALUE_CHANGED,
        nullptr
    );

    // Save button
    make_btn(tab, "Save Config", [](lv_event_t *) { settings::save(); });

    // Factory reset
    make_btn(tab, "Factory Reset", [](lv_event_t *) { settings::factoryReset(); });
}

} // namespace ui
