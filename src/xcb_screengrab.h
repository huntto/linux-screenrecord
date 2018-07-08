#ifndef XCB_SCREENGRAB_H_
#define XCB_SCREENGRAB_H_

#include "screengrab.h"

#include <xcb/shm.h>
#include <xcb/xcb.h>
#include <xcb/xcb_image.h>
namespace video {
namespace grab {

class XcbScreenGrab : public ScreenGrab {
public:
    XcbScreenGrab();
    virtual ~XcbScreenGrab() = default;

    virtual bool          Init(uint32_t& width, uint32_t& height, uint32_t& stride) override;
    virtual frame::Image& Grab(bool draw_mouse) override;
    virtual bool          Destroy() override;

private:
    xcb_connection_t*      connection_;
    xcb_screen_t*          screen_;
    xcb_shm_segment_info_t shminfo_;

    video::frame::Image image_;

    bool inited_;

    void drawMouse();
};

}  // namespace grab
}  // namespace video

#endif  // XCB_SCREENGRAB_H_