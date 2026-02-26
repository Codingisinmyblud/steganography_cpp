#include "steg/dwt_embedder.hpp"

namespace steg {

Image DWTImageEmbedder::embed(const Image& cover, const std::vector<uint8_t>& payload, uint64_t seed) {
    // Stub
    return cover;
}

std::vector<uint8_t> DWTImageEmbedder::extract(const Image& stego, uint64_t seed, size_t expected_size) {
    // Stub
    return {};
}

} // namespace steg
