#pragma once
#include <LovyanGFX.hpp>

// Configure these pins for your wiring:
#define LCD_PIN_MOSI 11
#define LCD_PIN_MISO 13
#define LCD_PIN_SCLK 12
#define LCD_PIN_CS 10
#define LCD_PIN_DC 21
#define LCD_PIN_RST 18
#define LCD_PIN_BL 4

class LGFX_ILI9341 : public lgfx::LGFX_Device {
    lgfx::Panel_ILI9341 _panel;
    lgfx::Bus_SPI _bus;
    lgfx::Light_PWM _light;

public:
    LGFX_ILI9341() {
        { // SPI bus
            auto cfg = _bus.config();
            cfg.spi_host = SPI2_HOST; // ESP32-S3 HSPI (FSPI/HSPI mapping ok)
            cfg.spi_mode = 0;
            cfg.freq_write = 80000000; // 80 MHz if wiring is short/solid; drop to 40 MHz if unstable
            cfg.freq_read = 20000000;
            cfg.spi_3wire = true;
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
            cfg.panel_width = 240;
            cfg.panel_height = 320;
            cfg.offset_x = 0;
            cfg.offset_y = 0;
            cfg.offset_rotation = 0;
            cfg.readable = true;
            cfg.invert = false;
            cfg.rgb_order = false;
            cfg.dlen_16bit = false;
            cfg.bus_shared = true;
            _panel.config(cfg);
        }
        { // Backlight (optional)
            auto cfg = _light.config();
            cfg.pin_bl = LCD_PIN_BL;
            cfg.invert = false;
            cfg.freq = 44100;
            cfg.pwm_channel = 7;
            _light.config(cfg);
            _panel.setLight(&_light);
        }
        setPanel(&_panel);
    }
};
