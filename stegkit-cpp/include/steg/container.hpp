#pragma once
#include <vector>
#include <cstdint>
#include <string>

namespace steg {

struct PackedContainer {
    uint32_t magic;
    uint32_t payload_size;
    uint32_t checksum;
    std::vector<uint8_t> payload_data;
};

class Container {
public:
    static std::vector<uint8_t> pack(const std::vector<uint8_t>& payload);
    static bool unpack(const std::vector<uint8_t>& packed_data, std::vector<uint8_t>& out_payload);
};

} // namespace steg
