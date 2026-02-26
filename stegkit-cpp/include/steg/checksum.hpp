#pragma once
#include <vector>
#include <cstdint>

namespace steg {

class Checksum {
public:
    static uint32_t compute(const std::vector<uint8_t>& data);
};

} // namespace steg
