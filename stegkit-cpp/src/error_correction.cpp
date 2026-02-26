#include "steg/error_correction.hpp"

namespace steg {

std::vector<uint8_t> ErrorCorrection::encode(const std::vector<uint8_t>& data) {
    // Stub: No actual ECC yet. Returns data as-is.
    return data;
}

std::vector<uint8_t> ErrorCorrection::decode(const std::vector<uint8_t>& data) {
    // Stub: Returns data as-is.
    return data;
}

} // namespace steg
