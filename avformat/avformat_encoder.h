#ifndef AVFORMAT_ENCODER_H_
#define AVFORMAT_ENCODER_H_

#include <stdint.h>
#include <string>
#include <vector>

namespace avformat {

namespace encoder {

class AVFormatEncoder {
public:
    virtual int Init(const std::string file_name, int video_fps,
                     int video_width, int video_height) = 0;
    virtual int WriteVideoPacket(uint8_t* packet, int size, uint32_t timestamp,
                                 bool keyframe) = 0;
    virtual int WriteAudioPacket(uint8_t* packet, int size,
                                 uint32_t timestamp) = 0;

    virtual ~AVFormatEncoder() = default;
};

}  // namespace encoder
}  // namespace avformat

#endif  // AVFORMAT_ENCODER_H_
