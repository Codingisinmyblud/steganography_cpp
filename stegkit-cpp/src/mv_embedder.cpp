#include "steg/mv_embedder.hpp"

namespace steg {

void MVHidingVideoEmbedder::embed(VideoCodec& in_video, VideoCodec& out_video, const std::vector<uint8_t>& payload, uint64_t seed) {
    // Stub
}

std::vector<uint8_t> MVHidingVideoEmbedder::extract(VideoCodec& in_video, uint64_t seed, size_t expected_size) {
    // Stub
    return {};
}

} // namespace steg
