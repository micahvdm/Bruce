#include "services.h"

namespace app {
void services_init() {
    wifi::init();
    ble::init();
    rf::init();
    rfid::init();
    ir::init();
    scripts::init();
    files::init();
    settings::init();
}
void services_tick() {
    wifi::tick();
    ble::tick();
    rf::tick();
    rfid::tick();
    ir::tick();
    scripts::tick();
    files::tick();
    settings::tick();
}
} // namespace app
