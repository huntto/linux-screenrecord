#ifndef VIDEO_ENCODER_H_
#define VIDEO_ENCODER_H_

#include <stdint.h>
#include <memory>

#include "image.h"

namespace video {
namespace encoder {

class VideoEncoder {
public:
    virtual bool Init(int width, int height, uint32_t fps_numerator,
                      uint32_t fps_denominator, int bitrate) = 0;
    virtual void Destroy() = 0;
    virtual int Encode(std::shared_ptr<frame::Image> image, uint8_t** packet,
                       int& is_keyframe) = 0;
    virtual ~VideoEncoder() = default;
};

}  // namespace encoder
}  // namespace video

#endif  // VIDEO_ENCODER_H_
