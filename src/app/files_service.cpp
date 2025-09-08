#include "files_service.h"
#include <string>
#include <vector>

#if defined(__has_include)
// Umbrella includes (the file managers live as menu apps)
#if __has_include("core/menu_items/SDCardMenu.h")
#include "core/menu_items/SDCardMenu.h"
#endif
#if __has_include("core/menu_items/LittleFSMenu.h")
#include "core/menu_items/LittleFSMenu.h"
#endif

// File ops modules
#if __has_include("modules/files/files.h")
#include "modules/files/files.h"
#define HAVE_BRUCE_FILES 1
#endif
#if __has_include("modules/webui/webui.h")
#include "modules/webui/webui.h"
#endif
#if __has_include("modules/rf/subghz.h")
#include "modules/rf/subghz.h"
#endif
#endif

namespace app::files {

// —— Map to Bruce’s file UIs & helpers (edit if renamed) —— //
#ifdef HAVE_BRUCE_FILES
#ifndef MAP_SD_MANAGER_RUN
#define MAP_SD_MANAGER_RUN sd_manager_run
#endif
#ifndef MAP_LFS_MANAGER_RUN
#define MAP_LFS_MANAGER_RUN littlefs_manager_run
#endif

// Generic storage helpers used by CLI/WebUI and menus
#ifndef MAP_STORAGE_READ_TEXT
#define MAP_STORAGE_READ_TEXT storage_read_text // (path, String&)
#endif
#ifndef MAP_STORAGE_READ_BIN
#define MAP_STORAGE_READ_BIN storage_read_bin // (path, std::vector<uint8_t>&)
#endif
#ifndef MAP_STORAGE_WRITE_TEXT
#define MAP_STORAGE_WRITE_TEXT storage_write_text // (path, data, fsSel)
#endif
#ifndef MAP_STORAGE_COPY
#define MAP_STORAGE_COPY storage_copy
#endif
#ifndef MAP_STORAGE_REMOVE
#define MAP_STORAGE_REMOVE storage_remove
#endif
#ifndef MAP_STORAGE_MKDIRS
#define MAP_STORAGE_MKDIRS storage_mkdirs
#endif
#ifndef MAP_STORAGE_RENAME
#define MAP_STORAGE_RENAME storage_rename
#endif

// Viewers (text/binary); harmless no-ops if not present
#ifndef MAP_VIEWER_TEXT
#define MAP_VIEWER_TEXT viewer_open_text
#endif
#ifndef MAP_VIEWER_BIN
#define MAP_VIEWER_BIN viewer_open_binary
#endif

// Quick RF send for *.sub files
#ifndef MAP_SUBGHZ_TX_FROM_FILE
#define MAP_SUBGHZ_TX_FROM_FILE subghz_tx_from_file
#endif
#endif
// ———————————————————————————————————————————————— //

void init() {}
void tick() {}

void openSdManager() {
#ifdef HAVE_BRUCE_FILES MAP_SD_MANAGER_RUN();
#endif
}
void openLittleFsManager() {
#ifdef HAVE_BRUCE_FILES MAP_LFS_MANAGER_RUN();
#endif
}

bool readText(const std::string &path, std::string &out) {
#ifdef HAVE_BRUCE_FILES
    String tmp;
    bool ok = MAP_STORAGE_READ_TEXT(path.c_str(), tmp);
    if (ok) out.assign(tmp.c_str(), tmp.length());
    return ok;
#else
    (void)path;
    (void)out;
    return false;
#endif
}

bool readBinary(const std::string &path, std::vector<uint8_t> &out) {
#ifdef HAVE_BRUCE_FILES
    return MAP_STORAGE_READ_BIN(path.c_str(), out);
#else
    (void)path;
    out.clear();
    return false;
#endif
}

bool writeText(const std::string &path, const std::string &data, FileFs fsSel) {
#ifdef HAVE_BRUCE_FILES
    return MAP_STORAGE_WRITE_TEXT(path.c_str(), data.c_str(), fsSel);
#else
    (void)path;
    (void)data;
    (void)fsSel;
    return false;
#endif
}

bool copy(const std::string &from, const std::string &to) {
#ifdef HAVE_BRUCE_FILES return MAP_STORAGE_COPY(from.c_str(), to.c_str());
#else(void) from;
    (void)to;
    return false;
#endif
}
bool remove(const std::string &path) {
#ifdef HAVE_BRUCE_FILES return MAP_STORAGE_REMOVE(path.c_str());
#else(void) path;
    return false;
#endif
}
bool mkdirs(const std::string &path) {
#ifdef HAVE_BRUCE_FILES return MAP_STORAGE_MKDIRS(path.c_str());
#else(void) path;
    return false;
#endif
}
bool rename(const std::string &from, const std::string &to) {
#ifdef HAVE_BRUCE_FILES return MAP_STORAGE_RENAME(from.c_str(), to.c_str());
#else(void) from;
    (void)to;
    return false;
#endif
}

void viewFile(const std::string &path) {
#ifdef HAVE_BRUCE_FILES
    // Prefer text viewer; switch to BIN if you like
    MAP_VIEWER_TEXT(path.c_str());
#else
    (void)path;
#endif
}

void txRfFromSub(const std::string &path) {
#ifdef HAVE_BRUCE_FILES
    MAP_SUBGHZ_TX_FROM_FILE(path.c_str());
#else
    (void)path;
#endif
}

} // namespace app::files
