#pragma once
#include "ble_service.h"
#include "common.h"
#include "files_service.h"
#include "ir_service.h"
#include "rf_service.h"
#include "rfid_service.h"
#include "scripts_service.h"
#include "settings_service.h"
#include "wifi_service.h"

namespace app {
// Optional global lifecycle if you want a single entrypoint
void services_init(); // calls each module's init
void services_tick(); // lightweight per-loop service tick (optional)
} // namespace app
