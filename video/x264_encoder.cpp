#include "x264_encoder.h"

#include <iostream>
#include "libyuv.h"

namespace video {
namespace encoder {

X264Encoder::~X264Encoder() {}

bool X264Encoder::Init(int width, int height, uint32_t fps_numerator,
                       uint32_t fps_denominator, int bitrate) {
    x264_param_t x264_param;
    x264_param_default_preset(&x264_param, "ultrafast", "zerolatency");

    x264_param.i_width = width;
    x264_param.i_height = height;
    x264_param.i_fps_num = fps_numerator;
    x264_param.i_fps_den = fps_denominator;
    x264_param.i_csp = X264_CSP_NV12;
    x264_param.i_threads = 1;
    x264_param.i_keyint_max = 30;
    x264_param.i_log_level = X264_LOG_WARNING;
    x264_param.rc.i_rc_method = X264_RC_ABR;
    x264_param.rc.b_filler = 0;
    x264_param.rc.i_bitrate = bitrate;

    x264_encoder_ = x264_encoder_open(&x264_param);
    if (!x264_encoder_) {
        return false;
    }

    x264_picture_alloc(&input_picture_, X264_CSP_NV12, width, height);

    return true;
}
void X264Encoder::Destroy() {
    if (!inited_) {
        return;
    }
    if (x264_encoder_) {
        x264_encoder_close(x264_encoder_);
        x264_encoder_ = nullptr;
    }

    x264_picture_clean(&input_picture_);

    inited_ = false;
}

int X264Encoder::Encode(std::shared_ptr<frame::Image> image, uint8_t** packet,
                        int& is_keyframe) {
    switch (image->GetFormat()) {
        case frame::Image::Format::FMT_0RGB32:
            libyuv::ARGBToNV12(
                    image->GetData(), image->GetStride(),
                    input_picture_.img.plane[0], input_picture_.img.i_stride[0],
                    input_picture_.img.plane[1], input_picture_.img.i_stride[1],
                    image->GetWidth(), image->GetHeight());
            break;
        default:
            return 0;
    }

    int frame_size = 0;
    x264_picture_t pic_out;
    x264_nal_t* nal;
    int i_nal;
    frame_size = x264_encoder_encode(x264_encoder_, &nal, &i_nal,
                                     &input_picture_, &pic_out);
    is_keyframe = pic_out.b_keyframe;
    if (frame_size) {
        *packet = nal[0].p_payload;
    }
    return frame_size;
}

}  // namespace encoder
}  // namespace video
