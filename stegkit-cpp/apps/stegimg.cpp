#include <iostream>
#include <string>
#include "cxxopts.hpp"
#include "steg/pipeline.hpp"
#include "steg/lsb_embedder.hpp"
#include "steg/dct_embedder.hpp"

using namespace steg;

int main(int argc, char* argv[]) {
    cxxopts::Options options("stegimg", "Image steganography tool");
    options.add_options()
        ("m,mode", "Mode: hide or extract", cxxopts::value<std::string>())
        ("i,in", "Input cover image or stego image", cxxopts::value<std::string>())
        ("p,payload", "Payload file to hide", cxxopts::value<std::string>()->default_value(""))
        ("o,out", "Output file (stego image or extracted payload)", cxxopts::value<std::string>())
        ("k,pass", "Password", cxxopts::value<std::string>()->default_value("default_pass"))
        ("a,algo", "Algorithm: lsb or dct", cxxopts::value<std::string>()->default_value("lsb"))
        ("h,help", "Print usage");

    auto result = options.parse(argc, argv);
    if (result.count("help") || !result.count("mode") || !result.count("in") || !result.count("out")) {
        std::cout << options.help() << std::endl;
        return 0;
    }

    std::string mode = result["mode"].as<std::string>();
    std::string in_file = result["in"].as<std::string>();
    std::string out_file = result["out"].as<std::string>();
    std::string password = result["pass"].as<std::string>();
    std::string algo = result["algo"].as<std::string>();

    std::unique_ptr<ImageEmbedder> embedder;
    if (algo == "lsb") {
        embedder = std::make_unique<LSBImageEmbedder>();
    } else {
        embedder = std::make_unique<DCTImageEmbedder>();
    }

    try {
        if (mode == "hide") {
            if (!result.count("payload")) {
                std::cerr << "Payload file required for hiding" << std::endl;
                return 1;
            }
            std::string payload_file = result["payload"].as<std::string>();
            Pipeline::hide_in_image(*embedder, in_file, out_file, payload_file, password);
            std::cout << "Successfully hidden payload into " << out_file << std::endl;
        } else if (mode == "extract") {
            Pipeline::extract_from_image(*embedder, in_file, out_file, password);
            std::cout << "Attempted extraction to " << out_file << std::endl;
        } else {
            std::cerr << "Unknown mode: " << mode << std::endl;
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
