#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "steg/lsb_embedder.hpp"

using namespace steg;

TEST_CASE("LSB image embedding sanity") {
    // We cannot create a proper image test without a real filesystem mock or manual in-memory
    Image cover;
    cover.width = 10;
    cover.height = 10;
    cover.channels = 3;
    cover.data.resize(300, 0); // 10x10x3 = 300 bytes

    LSBImageEmbedder embedder;
    std::vector<uint8_t> payload = {0xAA, 0x55, 0x12, 0x34}; // 4 bytes = 32 bits
    
    Image stego = embedder.embed(cover, payload, 12345);
    CHECK(stego.data.size() == cover.data.size());
    
    std::vector<uint8_t> extracted = embedder.extract(stego, 12345, payload.size());
    CHECK(extracted.size() == payload.size());
    for (size_t i = 0; i < payload.size(); ++i) {
        CHECK(extracted[i] == payload[i]);
    }
}
