#include "steg/dct_embedder.hpp"

namespace steg {

Image DCTImageEmbedder::embed(const Image& cover, const std::vector<uint8_t>& payload, uint64_t seed) {
    // Stub
    return cover;
}

std::vector<uint8_t> DCTImageEmbedder::extract(const Image& stego, uint64_t seed, size_t expected_size) {
    // Stub
    return {};
}

} // namespace steg
