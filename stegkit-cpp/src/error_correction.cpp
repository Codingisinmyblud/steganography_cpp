#include "steg/error_correction.hpp"
#include <iostream>

namespace steg {

// Hamming(8,4) SECDED:
// Data bits: d1, d2, d3, d4
// Parity bits: p1, p2, p3
// Overall parity: p4
// Bit order: p1, p2, d1, p3, d2, d3, d4, p4
// index:     0,  1,  2,  3,  4,  5,  6,  7

static uint8_t encode_nibble(uint8_t data) {
    uint8_t d1 = (data >> 3) & 1;
    uint8_t d2 = (data >> 2) & 1;
    uint8_t d3 = (data >> 1) & 1;
    uint8_t d4 = (data >> 0) & 1;

    uint8_t p1 = d1 ^ d2 ^ d4;
    uint8_t p2 = d1 ^ d3 ^ d4;
    uint8_t p3 = d2 ^ d3 ^ d4;

    uint8_t encoded = (p1 << 7) | (p2 << 6) | (d1 << 5) | (p3 << 4) | (d2 << 3) | (d3 << 2) | (d4 << 1);
    
    // Calculate overall parity (even parity)
    uint8_t parity = 0;
    for (int i = 1; i < 8; ++i) {
        parity ^= ((encoded >> i) & 1);
    }
    encoded |= parity;
    
    return encoded;
}

static bool decode_nibble(uint8_t encoded, uint8_t& out_data) {
    uint8_t p1 = (encoded >> 7) & 1;
    uint8_t p2 = (encoded >> 6) & 1;
    uint8_t d1 = (encoded >> 5) & 1;
    uint8_t p3 = (encoded >> 4) & 1;
    uint8_t d2 = (encoded >> 3) & 1;
    uint8_t d3 = (encoded >> 2) & 1;
    uint8_t d4 = (encoded >> 1) & 1;
    uint8_t p4 = (encoded >> 0) & 1;

    uint8_t s1 = p1 ^ d1 ^ d2 ^ d4;
    uint8_t s2 = p2 ^ d1 ^ d3 ^ d4;
    uint8_t s3 = p3 ^ d2 ^ d3 ^ d4;

    uint8_t syndrome = (s3 << 2) | (s2 << 1) | s1;

    uint8_t parity = 0;
    for (int i = 1; i < 8; ++i) {
        parity ^= ((encoded >> i) & 1);
    }

    bool two_errors = false;
    if (syndrome != 0) {
        if (parity == p4) {
            // Double error detected, unrecoverable
            two_errors = true;
        } else {
            // Single error, correctable
            int error_bit = syndrome;
            // Hamming bit position to flip:
            int shift = -1;
            if (error_bit == 1) shift = 7;
            else if (error_bit == 2) shift = 6;
            else if (error_bit == 3) shift = 5; // d1 doesn't match standard mapping perfectly if we use custom, but let's map:
            // Standard Hamming(7,4) positions:
            // 1: p1, 2: p2, 3: d1, 4: p3, 5: d2, 6: d3, 7: d4
            if (error_bit == 3) d1 ^= 1;
            else if (error_bit == 5) d2 ^= 1;
            else if (error_bit == 6) d3 ^= 1;
            else if (error_bit == 7) d4 ^= 1;
        }
    }

    out_data = (d1 << 3) | (d2 << 2) | (d3 << 1) | d4;
    return !two_errors; // Return true if it was perfectly fine or successfully corrected 1 bit
}

std::vector<uint8_t> ErrorCorrection::encode(const std::vector<uint8_t>& data) {
    std::vector<uint8_t> result;
    result.reserve(data.size() * 2);

    for (uint8_t byte : data) {
        uint8_t high_nibble = (byte >> 4) & 0x0F;
        uint8_t low_nibble = byte & 0x0F;

        result.push_back(encode_nibble(high_nibble));
        result.push_back(encode_nibble(low_nibble));
    }
    return result;
}

std::vector<uint8_t> ErrorCorrection::decode(const std::vector<uint8_t>& data) {
    std::vector<uint8_t> result;
    if (data.size() % 2 != 0) {
        // Encoded data must be even length
        return result; 
    }

    result.reserve(data.size() / 2);

    for (size_t i = 0; i < data.size(); i += 2) {
        uint8_t high_nibble, low_nibble;
        bool b1 = decode_nibble(data[i], high_nibble);
        bool b2 = decode_nibble(data[i+1], low_nibble);

        if (!b1 || !b2) {
            std::cerr << "Warning: Unrecoverable double-bit error detected in ECC.\n";
        }

        result.push_back((high_nibble << 4) | low_nibble);
    }

    return result;
}

} // namespace steg
