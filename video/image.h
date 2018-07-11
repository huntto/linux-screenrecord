#ifndef IMAGE_H_
#define IMAGE_H_

#include <stdint.h>

namespace video {
namespace frame {
enum class ImageFormat { FMT_0BGR };
struct Image {
    int width;
    int height;
    int stride;
    uint8_t* data;
    ImageFormat fmt;
};
}  // namespace frame
}  // namespace video

#endif  // IMAGE_H_
