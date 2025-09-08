#include "rf_service.h"
#include <string>
#include <vector>

namespace app::rf {

// —— Map to Bruce’s RF entrypoints (edit here if your branch renamed them) —— //
#ifdef HAVE_BRUCE_RF
#ifndef MAP_RF_SCAN_COPY_RUN
#define MAP_RF_SCAN_COPY_RUN rf_scan_copy_run
#endif
#ifndef MAP_RF_RECORD_RAW_RUN
#define MAP_RF_RECORD_RAW_RUN rf_record_raw_run
#endif
#ifndef MAP_RF_REPLAY_RUN
#define MAP_RF_REPLAY_RUN rf_replay_run
#endif
#ifndef MAP_RF_SPECTRUM_RUN
#define MAP_RF_SPECTRUM_RUN rf_spectrum_run
#endif
#ifndef MAP_RF_SQUAREWAVE_SPECTRUM_RUN
#define MAP_RF_SQUAREWAVE_SPECTRUM_RUN rf_squarewave_spectrum_run
#endif
#ifndef MAP_RF_JAMMER_FULL_RUN
#define MAP_RF_JAMMER_FULL_RUN rf_jammer_full_run
#endif
#ifndef MAP_RF_JAMMER_INTERMITTENT_RUN
#define MAP_RF_JAMMER_INTERMITTENT_RUN rf_jammer_intermittent_run
#endif

// SubGhz helpers (wiki: `subghz tx_from_file file.sub`)
#ifndef MAP_SUBGHZ_TX_FROM_FILE
#define MAP_SUBGHZ_TX_FROM_FILE subghz_tx_from_file
#endif
#ifndef MAP_SUBGHZ_TX_HEX
#define MAP_SUBGHZ_TX_HEX subghz_tx // (hexKey, freq, te, count)
#endif
#endif
// ———————————————————————————————————————————————————————————————— //

void init() {}
void tick() {}

void openScanCopy() {
#ifdef HAVE_BRUCE_RF MAP_RF_SCAN_COPY_RUN();
#endif
}
void openRecordRaw() {
#ifdef HAVE_BRUCE_RF MAP_RF_RECORD_RAW_RUN();
#endif
}
void replayLast() {
#ifdef HAVE_BRUCE_RF MAP_RF_REPLAY_RUN();
#endif
}
void openSpectrum() {
#ifdef HAVE_BRUCE_RF MAP_RF_SPECTRUM_RUN();
#endif
}
void openSquarewaveSpectrum() {
#ifdef HAVE_BRUCE_RF MAP_RF_SQUAREWAVE_SPECTRUM_RUN();
#endif
}
void jammerFull() {
#ifdef HAVE_BRUCE_RF MAP_RF_JAMMER_FULL_RUN();
#endif
}
void jammerIntermittent() {
#ifdef HAVE_BRUCE_RF MAP_RF_JAMMER_INTERMITTENT_RUN();
#endif
}

bool txFromSubFile(const std::string &path) {
#ifdef HAVE_BRUCE_RF
    return MAP_SUBGHZ_TX_FROM_FILE(path.c_str());
#else
    (void)path;
    return false;
#endif
}

bool txCustom(const std::string &hexKey, uint32_t hz, int te, int count) {
#ifdef HAVE_BRUCE_RF
    return MAP_SUBGHZ_TX_HEX(hexKey.c_str(), hz, te, count);
#else
    (void)hexKey;
    (void)hz;
    (void)te;
    (void)count;
    return false;
#endif
}

} // namespace app::rf
