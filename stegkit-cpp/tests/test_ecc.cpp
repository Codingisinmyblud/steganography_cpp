#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "steg/error_correction.hpp"

using namespace steg;

TEST_CASE("Error Correction API (Stubbed pass-through)") {
    // Current implementation is a pass-through stub, test that it doesn't mangle data
    std::vector<uint8_t> payload = {0x01, 0x02, 0x03, 0xFF};
    
    std::vector<uint8_t> encoded = ErrorCorrection::encode(payload);
    CHECK(encoded == payload);

    std::vector<uint8_t> decoded = ErrorCorrection::decode(encoded);
    CHECK(decoded == payload);
}
