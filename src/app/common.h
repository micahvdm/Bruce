#pragma once
#include <Arduino.h>
#include <functional>
#include <stddef.h>
#include <stdint.h>
#include <string>
#include <vector>

namespace app {

enum class Status : uint8_t {
    Ok = 0,
    Busy,
    NotFound,
    InvalidArg,
    NotSupported,
    IOError,
    Denied,
    Timeout,
    Unknown
};

inline const char *status_str(Status s) {
    switch (s) {
        case Status::Ok: return "Ok";
        case Status::Busy: return "Busy";
        case Status::NotFound: return "NotFound";
        case Status::InvalidArg: return "InvalidArg";
        case Status::NotSupported: return "NotSupported";
        case Status::IOError: return "IOError";
        case Status::Denied: return "Denied";
        case Status::Timeout: return "Timeout";
        default: return "Unknown";
    }
}

#ifdef ARDUINO
#include <Arduino.h>
inline uint32_t now_ms() { return millis(); }
#else
#include <chrono>
inline uint32_t now_ms() {
    static const auto t0 = std::chrono::steady_clock::now();
    auto d = std::chrono::steady_clock::now() - t0;
    return (uint32_t)std::chrono::duration_cast<std::chrono::milliseconds>(d).count();
}
#endif

} // namespace app
