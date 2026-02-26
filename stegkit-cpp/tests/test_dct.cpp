#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "steg/dct_embedder.hpp"

using namespace steg;

TEST_CASE("DCT image embedding sanity") {
    Image cover;
    cover.width = 16;
    cover.height = 16;
    cover.channels = 3;
    // Fill with some gradient background to avoid all zeros
    for (int y = 0; y < 16; ++y) {
        for (int x = 0; x < 16; ++x) {
            cover.data.push_back(y * 10);
            cover.data.push_back(x * 10);
            cover.data.push_back(128);
        }
    }

    DCTImageEmbedder embedder;
    std::vector<uint8_t> payload = {0xAA, 0x55, 0x12, 0x34}; // 32 bits
    
    // 16x16 = 4 blocks. 4 blocks * 15 mid_freqs = 60 bits capacity. 32 bits fit easily.
    Image stego = embedder.embed(cover, payload, 12345);
    CHECK(stego.data.size() == cover.data.size());
    
    std::vector<uint8_t> extracted = embedder.extract(stego, 12345, payload.size());
    CHECK(extracted.size() == payload.size());
    for (size_t i = 0; i < payload.size(); ++i) {
        CHECK(extracted[i] == payload[i]);
    }
}
