#pragma once
#include <string>
#include <vector>
#include "embedder.hpp"

namespace steg {

class Pipeline {
public:
    static void hide_in_image(ImageEmbedder& embedder, const std::string& cover_path, const std::string& stego_path, const std::string& payload_path, const std::string& password);
    static void extract_from_image(ImageEmbedder& embedder, const std::string& stego_path, const std::string& output_path, const std::string& password);

    static void hide_in_video(VideoEmbedder& embedder, const std::string& cover_path, const std::string& stego_path, const std::string& payload_path, const std::string& password);
    static void extract_from_video(VideoEmbedder& embedder, const std::string& stego_path, const std::string& output_path, const std::string& password);
};

} // namespace steg
