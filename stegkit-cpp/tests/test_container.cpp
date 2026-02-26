#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "steg/container.hpp"

using namespace steg;

TEST_CASE("Container pack and unpack") {
    std::vector<uint8_t> payload = {1, 2, 3, 4, 10, 20, 30};
    auto packed = Container::pack(payload);
    
    std::vector<uint8_t> out_payload;
    bool success = Container::unpack(packed, out_payload);
    
    CHECK(success == true);
    CHECK(out_payload == payload);
}

TEST_CASE("Container invalid unpack") {
    std::vector<uint8_t> random_data = {0, 1, 2, 3, 4, 5, 6, 7};
    std::vector<uint8_t> out_payload;
    bool success = Container::unpack(random_data, out_payload);
    
    CHECK(success == false);
}
