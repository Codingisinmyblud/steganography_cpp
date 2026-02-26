#include "steg/checksum.hpp"

namespace steg {

uint32_t Checksum::compute(const std::vector<uint8_t>& data) {
    // Simple Adler-32 implementation
    uint32_t a = 1, b = 0;
    const uint32_t MOD_ADLER = 65521;
    for (uint8_t byte : data) {
        a = (a + byte) % MOD_ADLER;
        b = (b + a) % MOD_ADLER;
    }
    return (b << 16) | a;
}

} // namespace steg
