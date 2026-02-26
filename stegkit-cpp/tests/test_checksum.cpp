#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "steg/checksum.hpp"

using namespace steg;

TEST_CASE("Adler-32 Checksum functionality") {
    std::vector<uint8_t> data1 = {'t', 'e', 's', 't'};
    std::vector<uint8_t> data2 = {'T', 'e', 's', 't'};
    std::vector<uint8_t> data3 = {};
    
    uint32_t c1 = Checksum::compute(data1);
    uint32_t c2 = Checksum::compute(data2);
    uint32_t c3 = Checksum::compute(data3);

    // Checksums should be deterministic and change based on data
    CHECK(c1 != c2);
    CHECK(c1 == Checksum::compute(data1));
    CHECK(c3 == 1); // Adler32 of empty is 1
}
