#include "steg/lsb_embedder.hpp"
#include "steg/bitstream.hpp"
#include "steg/rng.hpp"
#include <stdexcept>
#include <iostream>

namespace steg {

Image LSBImageEmbedder::embed(const Image& cover, const std::vector<uint8_t>& payload, uint64_t seed) {
    Image stego = cover;
    size_t required_bits = payload.size() * 8;
    if (required_bits > stego.data.size()) {
        throw std::runtime_error("Payload too large for cover image");
    }
    
    BitstreamReader reader(payload);
    LCG prng(seed);
    
    // We do a simple sequential LSB for testing, PRNG can be used for random walk
    size_t bit_idx = 0;
    while (!reader.eof()) {
        uint8_t bit;
        if (!reader.read_bit(bit)) break;
        
        // Ensure we don't exceed data size
        if (bit_idx >= stego.data.size()) break;
        
        stego.data[bit_idx] = (stego.data[bit_idx] & 0xFE) | bit;
        bit_idx++;
    }
    return stego;
}

std::vector<uint8_t> LSBImageEmbedder::extract(const Image& stego, uint64_t seed, size_t expected_size) {
    std::vector<uint8_t> extracted;
    size_t num_bits = expected_size * 8;
    if (num_bits > stego.data.size()) {
        num_bits = stego.data.size();
    }
    
    BitstreamWriter writer;
    for (size_t i = 0; i < num_bits; ++i) {
        uint8_t bit = stego.data[i] & 1;
        writer.write_bit(bit);
    }
    
    return writer.get_data();
}

} // namespace steg
