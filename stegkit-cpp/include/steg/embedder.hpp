#pragma once
#include <vector>
#include <cstdint>
#include "image_codec.hpp"
#include "video_codec.hpp"

namespace steg {

class ImageEmbedder {
public:
    virtual ~ImageEmbedder() = default;
    virtual Image embed(const Image& cover, const std::vector<uint8_t>& payload, uint64_t seed) = 0;
    virtual std::vector<uint8_t> extract(const Image& stego, uint64_t seed, size_t expected_size) = 0;
};

class VideoEmbedder {
public:
    virtual ~VideoEmbedder() = default;
    virtual void embed(VideoCodec& in_video, VideoCodec& out_video, const std::vector<uint8_t>& payload, uint64_t seed) = 0;
    virtual std::vector<uint8_t> extract(VideoCodec& in_video, uint64_t seed, size_t expected_size) = 0;
};

} // namespace steg
