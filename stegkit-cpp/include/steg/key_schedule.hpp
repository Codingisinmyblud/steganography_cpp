#pragma once
#include <string>
#include <cstdint>
#include <vector>

namespace steg {

class KeySchedule {
public:
    static uint64_t derive_seed(const std::string& passphrase);
};

} // namespace steg
