#ifndef SCREEN_GRAB_H
#define SCREEN_GRAB_H

#include <stdint.h>

#include "image.h"

namespace video {
namespace grab {

class ScreenGrab {
public:
    virtual ~ScreenGrab() = default;

    virtual bool Init(int& width, int& height, int& stride) = 0;
    virtual bool Destroy() = 0;
    virtual video::frame::Image& Grab(bool draw_mouse) = 0;
};

}  // namespace grab
}  // namespace video
#endif  // SCREEN_GRAB_H
