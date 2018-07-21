#ifndef FLV_ENCODER_H_
#define FLV_ENCODER_H_

#include "avformat_encoder.h"

#include <mutex>

#include "flv.h"

namespace avformat {
namespace encoder {

class FlvEncoder : public AVFormatEncoder {
public:
    FlvEncoder() : flv_handle_(nullptr), mutex_{} {}

    virtual int Init(const std::string file_name, int video_fps,
                     int video_width, int video_height) override;
    virtual int WriteVideoPacket(uint8_t* packet, int size, uint32_t timestamp,
                                 bool keyframe) override;
    virtual int WriteAudioPacket(uint8_t* packet, int size,
                                 uint32_t timestamp) override;

    virtual ~FlvEncoder() override;

private:
    flv_t* flv_handle_;
    std::mutex mutex_;
};
}  // namespace encoder
}  // namespace avformat

#endif
