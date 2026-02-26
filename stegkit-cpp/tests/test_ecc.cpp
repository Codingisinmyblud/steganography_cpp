#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "steg/error_correction.hpp"

using namespace steg;

TEST_CASE("Hamming(8,4) Error Correction") {
    std::vector<uint8_t> payload = {0x01, 0x02, 0x03, 0xFF, 0xAA};
    
    std::vector<uint8_t> encoded = ErrorCorrection::encode(payload);
    CHECK(encoded.size() == payload.size() * 2);

    std::vector<uint8_t> decoded_flawless = ErrorCorrection::decode(encoded);
    CHECK(decoded_flawless == payload);

    std::vector<uint8_t> corrupted = encoded;
    corrupted[0] ^= 0x04; 
    corrupted[3] ^= 0x80; 
    
    std::vector<uint8_t> decoded_corrected = ErrorCorrection::decode(corrupted);
    CHECK(decoded_corrected == payload);
}
