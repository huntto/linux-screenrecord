#ifndef X264_ENCODER_H_
#define X264_ENCODER_H_

#include "video_encoder.h"
#include "x264.h"

namespace video {
namespace encoder {

class X264Encoder : public VideoEncoder {
public:
    virtual bool Init(int width, int height, uint32_t fps_numerator,
                      uint32_t fps_denominator, int bitrate) override;
    virtual void Destroy() override;
    virtual int Encode(frame::Image& image, uint8_t** packet,
                       int& is_keyframe) override;
    virtual ~X264Encoder() override;

private:
    x264_t* x264_encoder_;
    x264_picture_t input_picture_;
    bool inited_;
};

}  // namespace encoder
}  // namespace video

#endif  // X264_ENCODER_H_
