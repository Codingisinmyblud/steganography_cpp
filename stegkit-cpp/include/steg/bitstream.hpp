#pragma once
#include <vector>
#include <cstdint>
#include <cstddef>

namespace steg {

class BitstreamWriter {
public:
    void write_bit(uint8_t bit);
    void write_byte(uint8_t byte);
    void write_bytes(const std::vector<uint8_t>& data);
    
    const std::vector<uint8_t>& get_data() const { return data_; }
    size_t bit_size() const { return bit_count_; }

private:
    std::vector<uint8_t> data_;
    size_t bit_count_ = 0;
};

class BitstreamReader {
public:
    explicit BitstreamReader(const std::vector<uint8_t>& data);
    
    bool read_bit(uint8_t& bit);
    bool read_byte(uint8_t& byte);
    bool read_bytes(std::vector<uint8_t>& dest, size_t count);
    
    bool eof() const;

private:
    const std::vector<uint8_t>& data_;
    size_t bit_pos_ = 0;
};

} // namespace steg
