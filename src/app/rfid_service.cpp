#include "rfid_service.h"

// ---- Module entry points implemented under src/modules/rfid/ ----
extern void rfid_read_run();
extern void rfid_write_run();
extern void rfid_clone_run();
extern void rfid_write_ndef_run();
extern void rfid_erase_run();
extern void rfid_save_file_run();
extern void rfid_load_file_run();
extern void rfid_emulate_run();
extern void amiibolink_run();
extern void chameleon_run();

extern void rfid_read_125k_run();

// Config bridge (core/config.cpp)
extern void setRfidModule(uint8_t moduleId);

void RFIDService::readTag() { rfid_read_run(); }
void RFIDService::writeTag() { rfid_write_run(); }
void RFIDService::cloneTag() { rfid_clone_run(); }
void RFIDService::writeNdef() { rfid_write_ndef_run(); }
void RFIDService::eraseTag() { rfid_erase_run(); }
void RFIDService::saveToFile() { rfid_save_file_run(); }
void RFIDService::loadFromFile() { rfid_load_file_run(); }
void RFIDService::emulateTag() { rfid_emulate_run(); }
void RFIDService::amiibolink() { amiibolink_run(); }
void RFIDService::chameleon() { chameleon_run(); }

void RFIDService::read125k() { rfid_read_125k_run(); }

void RFIDService::setRfidModule(uint8_t moduleId) { ::setRfidModule(moduleId); }
