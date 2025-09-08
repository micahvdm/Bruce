#include "app/services.h"
#include "ui/ui_app.h"
#include <Arduino.h>

#if defined(UI_LVGL)
static ui::UiApp ui;
#endif

void setup() {
    Serial.begin(115200);
    app::services_init();
#if defined(UI_LVGL)
    ui.begin();
#endif
}
void loop() {
    app::services_tick();
#if defined(UI_LVGL)
    ui.loop();
#endif
    delay(1);
}
