#ifndef SCREEN_GRAB_H
#define SCREEN_GRAB_H

#include <stdint.h>
#include <memory>
#include <vector>

#include "image.h"

namespace video {
namespace grab {

class ScreenGrab {
public:
    virtual ~ScreenGrab() = default;

    virtual bool Init() = 0;
    virtual bool Grab(std::shared_ptr<frame::Image> image, bool draw_mouse) = 0;
    virtual bool Destroy() = 0;
    virtual int GetWidth() = 0;
    virtual int GetHeight() = 0;
    virtual int GetStride() = 0;
    virtual frame::Image::Format GetFormat() = 0;
};

}  // namespace grab
}  // namespace video
#endif  // SCREEN_GRAB_H
