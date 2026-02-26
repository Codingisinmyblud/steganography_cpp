#include "steg/bitstream.hpp"

namespace steg {

void BitstreamWriter::write_bit(uint8_t bit) {
    size_t byte_idx = bit_count_ / 8;
    size_t bit_idx = bit_count_ % 8;
    if (byte_idx >= data_.size()) {
        data_.push_back(0);
    }
    if (bit) {
        data_[byte_idx] |= (1 << bit_idx);
    }
    bit_count_++;
}

void BitstreamWriter::write_byte(uint8_t byte) {
    for (int i = 0; i < 8; ++i) {
        write_bit((byte >> i) & 1);
    }
}

void BitstreamWriter::write_bytes(const std::vector<uint8_t>& data) {
    for (uint8_t b : data) {
        write_byte(b);
    }
}


BitstreamReader::BitstreamReader(const std::vector<uint8_t>& data) : data_(data) {}

bool BitstreamReader::read_bit(uint8_t& bit) {
    if (eof()) return false;
    size_t byte_idx = bit_pos_ / 8;
    size_t bit_idx = bit_pos_ % 8;
    bit = (data_[byte_idx] >> bit_idx) & 1;
    bit_pos_++;
    return true;
}

bool BitstreamReader::read_byte(uint8_t& byte) {
    byte = 0;
    for (int i = 0; i < 8; ++i) {
        uint8_t bit = 0;
        if (!read_bit(bit)) return false;
        byte |= (bit << i);
    }
    return true;
}

bool BitstreamReader::read_bytes(std::vector<uint8_t>& dest, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        uint8_t byte = 0;
        if (!read_byte(byte)) return false;
        dest.push_back(byte);
    }
    return true;
}

bool BitstreamReader::eof() const {
    return bit_pos_ >= data_.size() * 8;
}

} // namespace steg
