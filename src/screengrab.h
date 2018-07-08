#ifndef SCREENGRAB_H
#define SCREENGRAB_H

#include <stdint.h>

#include "image.h"

namespace video {
namespace grab {

class ScreenGrab {
public:
    virtual ~ScreenGrab() = default;

    virtual bool                 Init(uint32_t& width, uint32_t& height, uint32_t& stride) = 0;
    virtual bool                 Destroy()                                                 = 0;
    virtual video::frame::Image& Grab(bool draw_mouse)                                     = 0;
};

}  // namespace grab
}  // namespace video
#endif  // SCREENGRAB_H