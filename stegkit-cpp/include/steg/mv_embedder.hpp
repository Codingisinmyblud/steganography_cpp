#pragma once
#include "embedder.hpp"

namespace steg {

class MVHidingVideoEmbedder : public VideoEmbedder {
public:
    void embed(VideoCodec& in_video, VideoCodec& out_video, const std::vector<uint8_t>& payload, uint64_t seed) override;
    std::vector<uint8_t> extract(VideoCodec& in_video, uint64_t seed, size_t expected_size) override;
};

} // namespace steg
