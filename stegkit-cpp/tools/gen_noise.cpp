#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include "cxxopts.hpp"

int main(int argc, char* argv[]) {
    cxxopts::Options options("gen_noise", "Generate random noise for payload testing");
    options.add_options()
        ("s,size", "Size in bytes", cxxopts::value<size_t>()->default_value("1024"))
        ("o,out", "Output file", cxxopts::value<std::string>())
        ("h,help", "Print usage");

    auto result = options.parse(argc, argv);
    if (result.count("help") || !result.count("out")) {
        std::cout << options.help() << std::endl;
        return 0;
    }

    size_t size = result["size"].as<size_t>();
    std::string out_file = result["out"].as<std::string>();

    std::vector<uint8_t> data(size);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);

    for (size_t i = 0; i < size; ++i) {
        data[i] = static_cast<uint8_t>(dis(gen));
    }

    std::ofstream file(out_file, std::ios::binary);
    file.write(reinterpret_cast<const char*>(data.data()), data.size());

    std::cout << "Generated " << size << " bytes of noise to " << out_file << std::endl;
    return 0;
}
