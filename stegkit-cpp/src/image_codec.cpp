#include "steg/image_codec.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

namespace steg {

Image ImageCodec::load(const std::string& path, int desired_channels) {
    Image img;
    uint8_t* raw = stbi_load(path.c_str(), &img.width, &img.height, &img.channels, desired_channels);
    if (!raw) {
        throw std::runtime_error("Failed to load image: " + path);
    }
    if (desired_channels > 0) {
        img.channels = desired_channels;
    }
    size_t size = img.width * img.height * img.channels;
    img.data.assign(raw, raw + size);
    stbi_image_free(raw);
    return img;
}

void ImageCodec::save(const std::string& path, const Image& img) {
    int stride = img.width * img.channels;
    if (path.ends_with(".png")) {
        stbi_write_png(path.c_str(), img.width, img.height, img.channels, img.data.data(), stride);
    } else if (path.ends_with(".bmp")) {
        stbi_write_bmp(path.c_str(), img.width, img.height, img.channels, img.data.data());
    } else {
        throw std::runtime_error("Unsupported save format for: " + path);
    }
}

} // namespace steg
