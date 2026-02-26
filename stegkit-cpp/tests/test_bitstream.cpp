#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "steg/bitstream.hpp"

using namespace steg;

TEST_CASE("Bitstream IO") {
    BitstreamWriter writer;
    writer.write_bit(1);
    writer.write_bit(0);
    writer.write_bit(1);
    writer.write_bit(1);
    writer.write_byte(0x5A); // 01011010
    
    const auto& data = writer.get_data();
    
    BitstreamReader reader(data);
    uint8_t b;
    reader.read_bit(b); CHECK(b == 1);
    reader.read_bit(b); CHECK(b == 0);
    reader.read_bit(b); CHECK(b == 1);
    reader.read_bit(b); CHECK(b == 1);
    
    uint8_t full_byte;
    reader.read_byte(full_byte);
    CHECK(full_byte == 0x5A);
}
