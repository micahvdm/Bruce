#pragma once
#include <XPT2046_Touchscreen.h>

#define TOUCH_CS 9
#define TOUCH_IRQ 7

static XPT2046_Touchscreen ts(TOUCH_CS, TOUCH_IRQ);

inline void touch_init() {
    ts.begin();
    ts.setRotation(1); // match display rotation
}

inline bool touch_read_lvgl(lv_indev_drv_t *drv, lv_indev_data_t *data) {
    static lv_point_t last = {0, 0};
    if (ts.touched()) {
        auto p = ts.getPoint();
        // Map raw to screen: adjust if needed
        last.x = p.x;
        last.y = p.y;
        data->point = last;
        data->state = LV_INDEV_STATE_PRESSED;
    } else {
        data->state = LV_INDEV_STATE_RELEASED;
    }
    return false;
}
