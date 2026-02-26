#include "steg/video_codec.hpp"
#include <stdexcept>
#include <iostream>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
#include <libavutil/opt.h>
#include <libswscale/swscale.h>
}

namespace steg {

struct VideoCodec::Impl {
    std::string path;
    bool is_write;
    int width = 0;
    int height = 0;
    
    AVFormatContext* fmt_ctx = nullptr;
    AVCodecContext* codec_ctx = nullptr;
    AVStream* stream = nullptr;
    AVFrame* frame = nullptr;
    AVPacket* pkt = nullptr;
    SwsContext* sws_ctx = nullptr;
    AVFrame* rgb_frame = nullptr;

    int stream_idx = -1;

    ~Impl() {
        if (rgb_frame) {
            av_freep(&rgb_frame->data[0]);
            av_frame_free(&rgb_frame);
        }
        if (frame) av_frame_free(&frame);
        if (pkt) av_packet_free(&pkt);
        if (codec_ctx) avcodec_free_context(&codec_ctx);
        if (fmt_ctx && is_write) {
            if (!(fmt_ctx->oformat->flags & AVFMT_NOFILE)) {
                avio_closep(&fmt_ctx->pb);
            }
            avformat_free_context(fmt_ctx);
        } else if (fmt_ctx) {
            avformat_close_input(&fmt_ctx);
        }
        if (sws_ctx) sws_freeContext(sws_ctx);
    }
};

VideoCodec::VideoCodec(const std::string& path, bool is_write) 
    : pimpl(new Impl{path, is_write}) {
    
    pimpl->pkt = av_packet_alloc();
    pimpl->frame = av_frame_alloc();
    pimpl->rgb_frame = av_frame_alloc();

    if (!is_write) {
        if (avformat_open_input(&pimpl->fmt_ctx, path.c_str(), nullptr, nullptr) < 0) {
            throw std::runtime_error("Could not open input video: " + path);
        }
        if (avformat_find_stream_info(pimpl->fmt_ctx, nullptr) < 0) {
            throw std::runtime_error("Could not find stream info for: " + path);
        }

        // Find video stream
        for (unsigned int i = 0; i < pimpl->fmt_ctx->nb_streams; i++) {
            if (pimpl->fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
                pimpl->stream_idx = i;
                break;
            }
        }
        if (pimpl->stream_idx == -1) {
            throw std::runtime_error("No video stream found in: " + path);
        }

        pimpl->stream = pimpl->fmt_ctx->streams[pimpl->stream_idx];
        const AVCodec* decoder = avcodec_find_decoder(pimpl->stream->codecpar->codec_id);
        if (!decoder) throw std::runtime_error("Failed to find decoder");

        pimpl->codec_ctx = avcodec_alloc_context3(decoder);
        avcodec_parameters_to_context(pimpl->codec_ctx, pimpl->stream->codecpar);

        if (avcodec_open2(pimpl->codec_ctx, decoder, nullptr) < 0) {
            throw std::runtime_error("Failed to open codec");
        }

        pimpl->width = pimpl->codec_ctx->width;
        pimpl->height = pimpl->codec_ctx->height;

        pimpl->sws_ctx = sws_getContext(
            pimpl->width, pimpl->height, pimpl->codec_ctx->pix_fmt,
            pimpl->width, pimpl->height, AV_PIX_FMT_RGB24,
            SWS_BILINEAR, nullptr, nullptr, nullptr
        );

        int num_bytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24, pimpl->width, pimpl->height, 1);
        uint8_t* buffer = (uint8_t*)av_malloc(num_bytes * sizeof(uint8_t));
        av_image_fill_arrays(pimpl->rgb_frame->data, pimpl->rgb_frame->linesize, buffer,
                             AV_PIX_FMT_RGB24, pimpl->width, pimpl->height, 1);
    } else {
        // Output - simplified wrapper (defaults to MP4/H264)
        avformat_alloc_output_context2(&pimpl->fmt_ctx, nullptr, nullptr, path.c_str());
        if (!pimpl->fmt_ctx) throw std::runtime_error("Could not create output context");

        const AVCodec* encoder = avcodec_find_encoder(AV_CODEC_ID_MPEG4);
        if (!encoder) throw std::runtime_error("Could not find mpeg4 encoder");
        pimpl->stream = avformat_new_stream(pimpl->fmt_ctx, encoder);
        pimpl->codec_ctx = avcodec_alloc_context3(encoder);
        
        // Use realistic defaults. In a real app we'd inherit these from input VideoCodec
        pimpl->width = 1920; 
        pimpl->height = 1080;
        pimpl->codec_ctx->width = pimpl->width;
        pimpl->codec_ctx->height = pimpl->height;
        pimpl->codec_ctx->time_base = {1, 30};
        pimpl->codec_ctx->framerate = {30, 1};
        pimpl->codec_ctx->pix_fmt = AV_PIX_FMT_YUV420P;
        
        if (pimpl->fmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
            pimpl->codec_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

        if (avcodec_open2(pimpl->codec_ctx, encoder, nullptr) < 0)
            throw std::runtime_error("Cannot open video encoder");

        avcodec_parameters_from_context(pimpl->stream->codecpar, pimpl->codec_ctx);
        pimpl->stream->time_base = pimpl->codec_ctx->time_base;

        if (!(pimpl->fmt_ctx->oformat->flags & AVFMT_NOFILE)) {
            if (avio_open(&pimpl->fmt_ctx->pb, path.c_str(), AVIO_FLAG_WRITE) < 0)
                throw std::runtime_error("Could not open output file");
        }
        
        avformat_write_header(pimpl->fmt_ctx, nullptr);

        pimpl->sws_ctx = sws_getContext(
            pimpl->width, pimpl->height, AV_PIX_FMT_RGB24,
            pimpl->width, pimpl->height, pimpl->codec_ctx->pix_fmt,
            SWS_BILINEAR, nullptr, nullptr, nullptr
        );

        pimpl->frame->format = pimpl->codec_ctx->pix_fmt;
        pimpl->frame->width = pimpl->width;
        pimpl->frame->height = pimpl->height;
        av_frame_get_buffer(pimpl->frame, 32);
    }
}

VideoCodec::~VideoCodec() {
    if (pimpl->is_write && pimpl->fmt_ctx) {
        av_write_trailer(pimpl->fmt_ctx);
    }
    delete pimpl;
}

bool VideoCodec::read_frame(VideoFrame& out_frame) {
    if (pimpl->is_write) return false;

    while (av_read_frame(pimpl->fmt_ctx, pimpl->pkt) >= 0) {
        if (pimpl->pkt->stream_index == pimpl->stream_idx) {
            int response = avcodec_send_packet(pimpl->codec_ctx, pimpl->pkt);
            if (response >= 0) {
                response = avcodec_receive_frame(pimpl->codec_ctx, pimpl->frame);
                if (response >= 0) {
                    sws_scale(pimpl->sws_ctx, (uint8_t const * const *)pimpl->frame->data,
                              pimpl->frame->linesize, 0, pimpl->height,
                              pimpl->rgb_frame->data, pimpl->rgb_frame->linesize);
                    
                    out_frame.width = pimpl->width;
                    out_frame.height = pimpl->height;
                    
                    size_t size = pimpl->width * pimpl->height * 3;
                    out_frame.data.assign(pimpl->rgb_frame->data[0], pimpl->rgb_frame->data[0] + size);
                    av_packet_unref(pimpl->pkt);
                    return true;
                }
            }
        }
        av_packet_unref(pimpl->pkt);
    }
    return false;
}

bool VideoCodec::write_frame(const VideoFrame& frame) {
    if (!pimpl->is_write) return false;

    // We assume the incoming frame is RGB24 and we scale back to YUV420P
    int num_bytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24, pimpl->width, pimpl->height, 1);
    uint8_t* in_data[1] = { const_cast<uint8_t*>(frame.data.data()) };
    int in_linesize[1] = { 3 * pimpl->width };

    sws_scale(pimpl->sws_ctx, in_data, in_linesize, 0, pimpl->height,
              pimpl->frame->data, pimpl->frame->linesize);

    pimpl->frame->pts = pimpl->codec_ctx->frame_number;
    
    int response = avcodec_send_frame(pimpl->codec_ctx, pimpl->frame);
    if (response >= 0) {
        while (avcodec_receive_packet(pimpl->codec_ctx, pimpl->pkt) >= 0) {
            av_packet_rescale_ts(pimpl->pkt, pimpl->codec_ctx->time_base, pimpl->stream->time_base);
            pimpl->pkt->stream_index = pimpl->stream->index;
            av_interleaved_write_frame(pimpl->fmt_ctx, pimpl->pkt);
            av_packet_unref(pimpl->pkt);
        }
    }
    return true;
}

int VideoCodec::width() const { return pimpl->width; }
int VideoCodec::height() const { return pimpl->height; }

} // namespace steg
