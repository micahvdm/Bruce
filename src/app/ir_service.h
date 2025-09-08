#pragma once
#include "common.h"
#include <functional>
#include <string>
#include <vector>

namespace app::ir {
struct Signal {
    std::string proto;
    std::vector<uint16_t> raw;
    uint32_t carrier_khz;
};
using LearnCb = std::function<void(const Signal &sig, bool done)>;

void init();
void tick();
Status learn_async(LearnCb cb, uint32_t timeout_ms);
Status send(const Signal &sig);
} // namespace app::ir
