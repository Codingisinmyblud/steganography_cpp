#pragma once
#include "embedder.hpp"

namespace steg {

class DWTImageEmbedder : public ImageEmbedder {
public:
    Image embed(const Image& cover, const std::vector<uint8_t>& payload, uint64_t seed) override;
    std::vector<uint8_t> extract(const Image& stego, uint64_t seed, size_t expected_size) override;
};

} // namespace steg
