#pragma once
#include <string>
#include <vector>
#include <cstdint>

namespace steg {

struct VideoFrame {
    int width = 0;
    int height = 0;
    std::vector<uint8_t> data; // e.g. YUV420P or RGB
};

class VideoCodec {
public:
    VideoCodec(const std::string& path, bool is_write = false);
    ~VideoCodec();

    bool read_frame(VideoFrame& frame);
    bool write_frame(const VideoFrame& frame);
    
    int width() const;
    int height() const;

private:
    struct Impl;
    Impl* pimpl = nullptr;
};

} // namespace steg
