#ifndef IMAGE_H_
#define IMAGE_H_

#include <stdint.h>

namespace video {
namespace frame {
enum class ImageFormat { FMT_0BGR };
struct Image {
    uint32_t    width;
    uint32_t    height;
    uint32_t    stride;
    uint8_t*    data;
    ImageFormat fmt;
};
}  // namespace frame
}  // namespace video

#endif  // IMAGE_H_