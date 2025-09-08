#include "scripts_service.h"
#include <string>

#if defined(__has_include)
// Umbrella include
#if __has_include("core/menu_items/ScriptsMenu.h")
#include "core/menu_items/ScriptsMenu.h"
#endif

// Interpreter (JS)
#if __has_include("modules/bjs_interpreter/interpreter.h")
#include "modules/bjs_interpreter/interpreter.h"
#define HAVE_BRUCE_JS 1
#endif
#endif

namespace app::scripts {

// \u2014\u2014 Map to Interpreter JS APIs (per repo/wiki; JS only) \u2014\u2014 //
#ifdef HAVE_BRUCE_JS
#ifndef MAP_JS_RUN_FILE
#define MAP_JS_RUN_FILE js_run_file // (path)
#endif
#ifndef MAP_JS_RUN_CODE
#define MAP_JS_RUN_CODE js_run_code // (code)
#endif
#ifndef MAP_JS_PICKER
#define MAP_JS_PICKER js_open_picker
#endif
#endif
// \u2014\u2014\u2014\u2014\u2014\u2014\u2014\u2014\u2014\u2014\u2014\u2014\u2014\u2014\u2014\u2014\u2014\u2014\u2014\u2014\u2014\u2014\u2014\u2014\u2014\u2014\u2014\u2014\u2014\u2014\u2014\u2014\u2014\u2014\u2014\u2014\u2014\u2014\u2014\u2014\u2014\u2014\u2014\u2014\u2014\u2014\u2014\u2014
// //

void init() {}
void tick() {}

bool runFile(const std::string &path) {
#ifdef HAVE_BRUCE_JS
    return MAP_JS_RUN_FILE(path.c_str());
#else
    (void)path;
    return false;
#endif
}

bool runCode(const std::string &code) {
#ifdef HAVE_BRUCE_JS
    return MAP_JS_RUN_CODE(code.c_str());
#else
    (void)code;
    return false;
#endif
}

void openPicker() {
#ifdef HAVE_BRUCE_JS
    MAP_JS_PICKER();
#endif
}

} // namespace app::scripts
