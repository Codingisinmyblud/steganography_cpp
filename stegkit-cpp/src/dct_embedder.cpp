#include "steg/dct_embedder.hpp"
#include "steg/bitstream.hpp"
#include <cmath>
#include <algorithm>
#include <stdexcept>
#include <iostream>

namespace steg {

constexpr double PI = 3.14159265358979323846;
constexpr int Q = 16; // Quantization step

static void fdct8x8(const double in[8][8], double out[8][8]) {
    for (int u = 0; u < 8; ++u) {
        for (int v = 0; v < 8; ++v) {
            double sum = 0.0;
            for (int x = 0; x < 8; ++x) {
                for (int y = 0; y < 8; ++y) {
                    sum += in[x][y] * std::cos((2 * x + 1) * u * PI / 16.0) * std::cos((2 * y + 1) * v * PI / 16.0);
                }
            }
            double cu = (u == 0) ? 1.0 / std::sqrt(2.0) : 1.0;
            double cv = (v == 0) ? 1.0 / std::sqrt(2.0) : 1.0;
            out[u][v] = 0.25 * cu * cv * sum;
        }
    }
}

static void idct8x8(const double in[8][8], double out[8][8]) {
    for (int x = 0; x < 8; ++x) {
        for (int y = 0; y < 8; ++y) {
            double sum = 0.0;
            for (int u = 0; u < 8; ++u) {
                for (int v = 0; v < 8; ++v) {
                    double cu = (u == 0) ? 1.0 / std::sqrt(2.0) : 1.0;
                    double cv = (v == 0) ? 1.0 / std::sqrt(2.0) : 1.0;
                    sum += cu * cv * in[u][v] * std::cos((2 * x + 1) * u * PI / 16.0) * std::cos((2 * y + 1) * v * PI / 16.0);
                }
            }
            out[x][y] = 0.25 * sum;
        }
    }
}

// Coordinates of mid-frequencies to embed in (avoids DC and highest AC)
static const std::vector<std::pair<int,int>> MID_FREQS = {
    {1,2}, {2,1}, {3,0}, {0,4}, {1,3}, {2,2}, {3,1}, {4,0},
    {0,5}, {1,4}, {2,3}, {3,2}, {4,1}, {5,0}, {0,6}
};

Image DCTImageEmbedder::embed(const Image& cover, const std::vector<uint8_t>& payload, uint64_t seed) {
    Image stego = cover;
    int blocks_x = cover.width / 8;
    int blocks_y = cover.height / 8;
    
    size_t capacity_bits = blocks_x * blocks_y * MID_FREQS.size();
    if (payload.size() * 8 > capacity_bits) {
        throw std::runtime_error("Payload too large for DCT embedding capacity");
    }

    BitstreamReader reader(payload);
    
    for (int by = 0; by < blocks_y && !reader.eof(); ++by) {
        for (int bx = 0; bx < blocks_x && !reader.eof(); ++bx) {
            double spatial[8][8];
            for (int y = 0; y < 8; ++y) {
                for (int x = 0; x < 8; ++x) {
                    int px = bx * 8 + x;
                    int py = by * 8 + y;
                    int idx = (py * cover.width + px) * cover.channels;
                    // Operate on the Blue channel (idx + 0)
                    spatial[y][x] = cover.data[idx];
                }
            }

            double dct[8][8];
            fdct8x8(spatial, dct);

            // Embed into mid freqs
            for (auto [u, v] : MID_FREQS) {
                if (reader.eof()) break;
                
                uint8_t bit;
                reader.read_bit(bit);

                // Quantize
                int coeff = std::round(dct[u][v] / Q);
                
                // Embed in LSB
                coeff = (coeff & ~1) | bit;
                
                // Dequantize
                dct[u][v] = coeff * Q;
            }

            double ispatial[8][8];
            idct8x8(dct, ispatial);

            for (int y = 0; y < 8; ++y) {
                for (int x = 0; x < 8; ++x) {
                    int px = bx * 8 + x;
                    int py = by * 8 + y;
                    int idx = (py * cover.width + px) * cover.channels;
                    stego.data[idx] = static_cast<uint8_t>(std::clamp(std::round(ispatial[y][x]), 0.0, 255.0));
                }
            }
        }
    }

    return stego;
}

std::vector<uint8_t> DCTImageEmbedder::extract(const Image& stego, uint64_t seed, size_t expected_size) {
    int blocks_x = stego.width / 8;
    int blocks_y = stego.height / 8;

    BitstreamWriter writer;
    size_t target_bits = expected_size * 8;
    
    for (int by = 0; by < blocks_y && writer.bit_size() < target_bits; ++by) {
        for (int bx = 0; bx < blocks_x && writer.bit_size() < target_bits; ++bx) {
            double spatial[8][8];
            for (int y = 0; y < 8; ++y) {
                for (int x = 0; x < 8; ++x) {
                    int px = bx * 8 + x;
                    int py = by * 8 + y;
                    int idx = (py * stego.width + px) * stego.channels;
                    spatial[y][x] = stego.data[idx];
                }
            }

            double dct[8][8];
            fdct8x8(spatial, dct);

            for (auto [u, v] : MID_FREQS) {
                if (writer.bit_size() >= target_bits) break;

                int coeff = std::round(dct[u][v] / Q);
                writer.write_bit(coeff & 1);
            }
        }
    }

    return writer.get_data();
}

} // namespace steg
