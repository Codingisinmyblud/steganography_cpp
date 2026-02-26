#pragma once
#include <vector>
#include <string>
#include <cstdint>
#include <stdexcept>

namespace steg {

struct Image {
    int width = 0;
    int height = 0;
    int channels = 0;
    std::vector<uint8_t> data;
};

class ImageCodec {
public:
    static Image load(const std::string& path, int desired_channels = 0);
    static void save(const std::string& path, const Image& img);
};

} // namespace steg
