#include "ir_service.h"

#if defined(__has_include)
#if __has_include("modules/ir/ir.h")
#include "modules/ir/ir.h"
#define HAVE_BRUCE_IR 1
#endif
#if __has_include("modules/ir/TV-B-Gone.h")
#include "modules/ir/TV-B-Gone.h"
#endif
#if __has_include("modules/ir/custom_ir.h")
#include "modules/ir/custom_ir.h"
#endif
#if __has_include("modules/ir/ir_jammer.h")
#include "modules/ir/ir_jammer.h"
#endif
#if __has_include("modules/ir/ir_read.h")
#include "modules/ir/ir_read.h"
#endif
#endif

namespace app::ir {
void init() {}
void tick() {}

#ifdef HAVE_BRUCE_IR
#ifndef MAP_IR_LEARN
#define MAP_IR_LEARN bruce_ir_learn // (timeout_ms, timings*, &len, &khz, proto*, proto_len)
#endif
#ifndef MAP_IR_SEND
#define MAP_IR_SEND bruce_ir_send // (timings,len,khz,proto)
#endif
#endif

Status learn_async(LearnCb cb, uint32_t timeout_ms) {
    if (!cb) return Status::InvalidArg;
#if defined(HAVE_BRUCE_IR)
    uint16_t buf[512];
    size_t len = 512;
    uint32_t khz = 0;
    char proto[16]{0};
    bool ok = MAP_IR_LEARN(timeout_ms, buf, &len, &khz, proto, sizeof(proto));
    if (!ok) return Status::Timeout;
    cb(Signal{proto, std::vector<uint16_t>(buf, buf + len), khz}, true);
    return Status::Ok;
#else
    cb(
        Signal{
            "NEC", {9000, 4500, 560, 560, 560, 1690},
             38
    },
        true
    );
    return Status::Ok;
#endif
}

Status send(const Signal &sig) {
#if defined(HAVE_BRUCE_IR)
    return MAP_IR_SEND(sig.raw.data(), sig.raw.size(), sig.carrier_khz, sig.proto.c_str()) ? Status::Ok
                                                                                           : Status::IOError;
#else
    (void)sig;
    return Status::NotSupported;
#endif
}
} // namespace app::ir
