#ifndef VIDEO_ENCODER_H_
#define VIDEO_ENCODER_H_

#include <stdint.h>

#include "image.h"

namespace video {
namespace encoder {

class VideoEncoder {
public:
    virtual bool Init(uint32_t width, uint32_t height, uint32_t fps_numerator, uint32_t fps_denominator, uint32_t bitrate) = 0;
    virtual void Destroy()                                                                                                 = 0;
    virtual int  Encode(frame::Image& image, uint8_t** packet)                                                             = 0;
    virtual ~VideoEncoder()                                                                                                = default;
};

}  // namespace encoder
}  // namespace video

#endif  // VIDEO_ENCODER_H_