#ifndef XCB_SCREENGRAB_H_
#define XCB_SCREENGRAB_H_

#include "screengrab.h"

class XcbScreenGrab : public ScreenGrab
{
public:
    XcbScreenGrab();
    virtual ~XcbScreenGrab() = default;

    virtual bool Init(uint32_t &width, uint32_t &height, uint32_t &stride) override;
    virtual uint8_t* Grab(bool draw_cursor) override;
    virtual bool Destroy() override;

private:
    
};

#endif // XCB_SCREENGRAB_H_