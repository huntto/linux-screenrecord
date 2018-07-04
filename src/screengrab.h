#ifndef SCREENGRAB_H
#define SCREENGRAB_H

#include <stdint.h>

class ScreenGrab
{
public:
    virtual ~ScreenGrab() = default;

    virtual bool Init(uint32_t &width, uint32_t &height, uint32_t &stride) = 0;
    virtual uint8_t* Grab(bool draw_cursor) = 0;
    virtual bool Destroy() = 0;
};

#endif // SCREENGRAB_H