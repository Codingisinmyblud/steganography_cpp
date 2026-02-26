#include "steg/pipeline.hpp"
#include "steg/container.hpp"
#include "steg/key_schedule.hpp"
#include "steg/image_codec.hpp"
#include <fstream>
#include <iostream>

namespace steg {

// Helpers
static std::vector<uint8_t> read_file(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    return std::vector<uint8_t>((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}

static void write_file(const std::string& path, const std::vector<uint8_t>& data) {
    std::ofstream file(path, std::ios::binary);
    file.write(reinterpret_cast<const char*>(data.data()), data.size());
}

void Pipeline::hide_in_image(ImageEmbedder& embedder, const std::string& cover_path, const std::string& stego_path, const std::string& payload_path, const std::string& password) {
    Image cover = ImageCodec::load(cover_path);
    std::vector<uint8_t> payload = read_file(payload_path);
    std::vector<uint8_t> packed = Container::pack(payload);
    
    uint64_t seed = KeySchedule::derive_seed(password);
    Image stego = embedder.embed(cover, packed, seed);
    ImageCodec::save(stego_path, stego);
}

void Pipeline::extract_from_image(ImageEmbedder& embedder, const std::string& stego_path, const std::string& output_path, const std::string& password) {
    Image stego = ImageCodec::load(stego_path);
    uint64_t seed = KeySchedule::derive_seed(password);
    
    // We assume an upper bound or dynamic size finding logic - simplify for now by extracting max possible
    size_t max_bytes = stego.data.size() / 8;
    std::vector<uint8_t> extracted = embedder.extract(stego, seed, max_bytes);
    
    std::vector<uint8_t> payload;
    if (Container::unpack(extracted, payload)) {
        write_file(output_path, payload);
    } else {
        std::cerr << "Extraction failed (corrupt or wrong password)." << std::endl;
    }
}

void Pipeline::hide_in_video(VideoEmbedder& embedder, const std::string& cover_path, const std::string& stego_path, const std::string& payload_path, const std::string& password) {
    std::vector<uint8_t> payload = read_file(payload_path);
    std::vector<uint8_t> packed = Container::pack(payload);
    
    VideoCodec in_video(cover_path, false);
    VideoCodec out_video(stego_path, true);
    
    uint64_t seed = KeySchedule::derive_seed(password);
    embedder.embed(in_video, out_video, packed, seed);
}

void Pipeline::extract_from_video(VideoEmbedder& embedder, const std::string& stego_path, const std::string& output_path, const std::string& password) {
    VideoCodec in_video(stego_path, false);
    uint64_t seed = KeySchedule::derive_seed(password);
    
    // Simplification: We extract an arbitrarily large block and Container unpack cuts it off
    // In a production system we'd extract the header first to know exact sizes
    size_t max_bytes = 1024 * 1024 * 10; // Try extracting up to 10MB
    std::vector<uint8_t> extracted = embedder.extract(in_video, seed, max_bytes);
    
    std::vector<uint8_t> payload;
    if (Container::unpack(extracted, payload)) {
        write_file(output_path, payload);
    } else {
        std::cerr << "Extraction failed (corrupt or wrong password)." << std::endl;
    }
}

} // namespace steg
