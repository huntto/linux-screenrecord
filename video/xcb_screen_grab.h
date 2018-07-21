#ifndef XCB_SCREEN_GRAB_H_
#define XCB_SCREEN_GRAB_H_

#include "screen_grab.h"

#include <xcb/shm.h>
#include <xcb/xcb.h>
#include <xcb/xcb_image.h>
namespace video {

namespace frame {
class ShmImage : public Image {
public:
    ShmImage()
            : width_(0),
              height_(0),
              stride_(0),
              format_(Format::NONE),
              shmid_(-1),
              shmaddr_(nullptr) {}
    virtual ~ShmImage() override;

    virtual int GetWidth() override { return width_; }
    virtual int GetHeight() override { return height_; }

    virtual int GetStride() override { return stride_; }
    virtual Format GetFormat() override { return format_; }
    virtual uint8_t* GetData() override {
        return static_cast<uint8_t*>(shmaddr_);
    }

    virtual bool Init(int width, int height, Format format) override;

    int GetShmId() { return shmid_; }

private:
    int width_;
    int height_;
    int stride_;
    Format format_;

    int shmid_;
    void* shmaddr_;
};
}  // namespace frame

namespace grab {
class XcbScreenGrab : public ScreenGrab {
public:
    XcbScreenGrab();
    virtual ~XcbScreenGrab() override { Destroy(); }

    virtual bool Init() override;
    virtual bool Grab(std::shared_ptr<frame::Image> image,
                      bool draw_mouse) override;
    virtual bool Destroy() override;
    virtual int GetWidth() override { return width_; }
    virtual int GetHeight() override { return height_; }
    virtual int GetStride() override { return stride_; }
    virtual frame::Image::Format GetFormat() override { return format_; }

private:
    xcb_connection_t* connection_;
    xcb_screen_t* screen_;
    xcb_shm_seg_t shmseg_;
    int width_;
    int height_;
    int stride_;
    frame::Image::Format format_;

    void drawMouse();
};

}  // namespace grab
}  // namespace video

#endif  // XCB_SCREEN_GRAB_H_
