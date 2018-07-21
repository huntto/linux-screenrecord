#include "flv_encoder.h"

namespace avformat {
namespace encoder {

int FlvEncoder::Init(const std::string file_name, int video_fps,
                     int video_width, int video_height) {
    if (flv_handle_ == nullptr) {
        flv_handle_ = flv_init(file_name.c_str(), video_fps, video_width,
                               video_height);
        return 0;
    }
    return -1;
}

int FlvEncoder::WriteVideoPacket(uint8_t* packet, int size, uint32_t timestamp,
                                 bool keyframe) {
    if (flv_handle_) {
        std::lock_guard<std::mutex> lock(mutex_);
        return flv_write_video_packet(flv_handle_, keyframe, packet, size,
                                      timestamp);
    }
    return 0;
}

int FlvEncoder::WriteAudioPacket(uint8_t* packet, int size,
                                 uint32_t timestamp) {
    if (flv_handle_) {
        std::lock_guard<std::mutex> lock(mutex_);
        return flv_write_audio_packet(flv_handle_, packet, size, timestamp);
    }
    return 0;
}

FlvEncoder::~FlvEncoder() {
    if (flv_handle_ != nullptr) {
        flv_write_trailer(flv_handle_);
        flv_handle_ = nullptr;
    }
}
}  // namespace encoder
}  // namespace avformat
