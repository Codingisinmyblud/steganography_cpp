#include "steg/mv_embedder.hpp"
#include "steg/bitstream.hpp"
#include "steg/rng.hpp"
#include <stdexcept>

namespace steg {

void MVHidingVideoEmbedder::embed(VideoCodec& in_video, VideoCodec& out_video, const std::vector<uint8_t>& payload, uint64_t seed) {
    BitstreamReader reader(payload);
    LCG prng(seed); // Optional: shuffle bit stream order later

    VideoFrame frame;
    while (in_video.read_frame(frame)) {
        if (!reader.eof()) {
            // Embed bits into this frame's spatial RGB data using simple LSB for now, 
            // since actual MV manipulation in FFmpeg requires deep codec hacking
            size_t bits_to_embed = std::min<size_t>(frame.data.size(), 1024 * 8); // embed up to 8KB per frame
            
            for (size_t i = 0; i < bits_to_embed && !reader.eof(); ++i) {
                uint8_t bit;
                reader.read_bit(bit);
                frame.data[i] = (frame.data[i] & 0xFE) | bit;
            }
        }
        out_video.write_frame(frame);
    }

    if (!reader.eof()) {
        throw std::runtime_error("Payload too large for video duration");
    }
}

std::vector<uint8_t> MVHidingVideoEmbedder::extract(VideoCodec& in_video, uint64_t seed, size_t expected_size) {
    BitstreamWriter writer;
    size_t target_bits = expected_size * 8;
    LCG prng(seed);

    VideoFrame frame;
    while (in_video.read_frame(frame) && writer.bit_size() < target_bits) {
        size_t bits_to_extract = std::min<size_t>(frame.data.size(), 1024 * 8);
        
        for (size_t i = 0; i < bits_to_extract && writer.bit_size() < target_bits; ++i) {
            uint8_t bit = frame.data[i] & 1;
            writer.write_bit(bit);
        }
    }

    return writer.get_data();
}

} // namespace steg
