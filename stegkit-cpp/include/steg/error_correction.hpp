#pragma once
#include <vector>
#include <cstdint>

namespace steg {

class ErrorCorrection {
public:
    static std::vector<uint8_t> encode(const std::vector<uint8_t>& data);
    static std::vector<uint8_t> decode(const std::vector<uint8_t>& data);
};

} // namespace steg
