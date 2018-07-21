#ifndef IMAGE_H_
#define IMAGE_H_

#include <stdint.h>
#include <vector>

namespace video {
namespace frame {
class Image {
public:
    enum class Format {
        NONE,
        FMT_0RGB32,
        FMT_RGB24,
        FMT_RGB565,
        FMT_RGB555,
        FMT_RGB8
    };

    virtual int GetWidth() = 0;
    virtual int GetHeight() = 0;
    virtual int GetStride() = 0;
    virtual Format GetFormat() = 0;
    virtual uint8_t* GetData() = 0;
    virtual bool Init(int width, int height, Format format) = 0;

    virtual ~Image() = default;
};
}  // namespace frame
}  // namespace video

#endif  // IMAGE_H_
