#include "steg/dwt_embedder.hpp"
#include "steg/bitstream.hpp"
#include <stdexcept>
#include <cmath>
#include <algorithm>

namespace steg {

constexpr double DWT_Q = 4.0; // Quantization factor for DWT

Image DWTImageEmbedder::embed(const Image& cover, const std::vector<uint8_t>& payload, uint64_t seed) {
    Image stego = cover;
    int blocks_x = cover.width / 2;
    int blocks_y = cover.height / 2;

    size_t capacity_bits = blocks_x * blocks_y; // 1 bit per 2x2 block (in HH band)
    if (payload.size() * 8 > capacity_bits) {
        throw std::runtime_error("Payload too large for DWT embedding capacity");
    }

    BitstreamReader reader(payload);

    for (int by = 0; by < blocks_y && !reader.eof(); ++by) {
        for (int bx = 0; bx < blocks_x && !reader.eof(); ++bx) {
            // Get 2x2 block across Blue channel
            int idx00 = ((by * 2 + 0) * cover.width + (bx * 2 + 0)) * cover.channels;
            int idx10 = ((by * 2 + 0) * cover.width + (bx * 2 + 1)) * cover.channels;
            int idx01 = ((by * 2 + 1) * cover.width + (bx * 2 + 0)) * cover.channels;
            int idx11 = ((by * 2 + 1) * cover.width + (bx * 2 + 1)) * cover.channels;

            double p00 = cover.data[idx00];
            double p10 = cover.data[idx10];
            double p01 = cover.data[idx01];
            double p11 = cover.data[idx11];

            // Forward Haar 2D DWT
            double ll = (p00 + p10 + p01 + p11) / 4.0;
            double hl = (p00 - p10 + p01 - p11) / 4.0;
            double lh = (p00 + p10 - p01 - p11) / 4.0;
            double hh = (p00 - p10 - p01 + p11) / 4.0;

            // Embed 1 bit into the High-High (HH) frequency band
            uint8_t bit;
            reader.read_bit(bit);

            // Quantize
            int coeff = std::round(hh / DWT_Q);
            
            // Embed in LSB
            coeff = (coeff & ~1) | bit;
            
            // Dequantize
            hh = coeff * DWT_Q;

            // Inverse Haar 2D DWT
            double i00 = ll + hl + lh + hh;
            double i10 = ll - hl + lh - hh;
            double i01 = ll + hl - lh - hh;
            double i11 = ll - hl - lh + hh;

            stego.data[idx00] = static_cast<uint8_t>(std::clamp(std::round(i00), 0.0, 255.0));
            stego.data[idx10] = static_cast<uint8_t>(std::clamp(std::round(i10), 0.0, 255.0));
            stego.data[idx01] = static_cast<uint8_t>(std::clamp(std::round(i01), 0.0, 255.0));
            stego.data[idx11] = static_cast<uint8_t>(std::clamp(std::round(i11), 0.0, 255.0));
        }
    }

    return stego;
}

std::vector<uint8_t> DWTImageEmbedder::extract(const Image& stego, uint64_t seed, size_t expected_size) {
    int blocks_x = stego.width / 2;
    int blocks_y = stego.height / 2;

    BitstreamWriter writer;
    size_t target_bits = expected_size * 8;

    for (int by = 0; by < blocks_y && writer.bit_size() < target_bits; ++by) {
        for (int bx = 0; bx < blocks_x && writer.bit_size() < target_bits; ++bx) {
            
            int idx00 = ((by * 2 + 0) * stego.width + (bx * 2 + 0)) * stego.channels;
            int idx10 = ((by * 2 + 0) * stego.width + (bx * 2 + 1)) * stego.channels;
            int idx01 = ((by * 2 + 1) * stego.width + (bx * 2 + 0)) * stego.channels;
            int idx11 = ((by * 2 + 1) * stego.width + (bx * 2 + 1)) * stego.channels;

            double p00 = stego.data[idx00];
            double p10 = stego.data[idx10];
            double p01 = stego.data[idx01];
            double p11 = stego.data[idx11];

            double hh = (p00 - p10 - p01 + p11) / 4.0;

            int coeff = std::round(hh / DWT_Q);
            writer.write_bit(coeff & 1);
        }
    }

    return writer.get_data();
}

} // namespace steg
