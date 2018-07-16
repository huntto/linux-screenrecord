#include "xcb_screen_grab.h"

#include <stdio.h>
#include <sys/shm.h>
#include <xcb/shape.h>
#include <xcb/shm.h>
#include <xcb/xcb.h>
#include <xcb/xfixes.h>
#include <iostream>

namespace video {
namespace grab {

XcbScreenGrab::XcbScreenGrab() : inited_(false) {}

bool XcbScreenGrab::Init(int& width, int& height, int& stride) {
    if (inited_) {
        return true;
    }

    /* Open the connection to the X server */
    connection_ = xcb_connect(nullptr, nullptr);

    /* Get the first screen */
    const xcb_setup_t* setup = xcb_get_setup(connection_);
    xcb_screen_iterator_t iter = xcb_setup_roots_iterator(setup);
    screen_ = iter.data;

    /* Get bpp */
    const xcb_format_t* fmt = xcb_setup_pixmap_formats(setup);
    int length = xcb_setup_pixmap_formats_length(setup);
    int bpp = 0;
    while (length--) {
        if (screen_->root_depth == fmt->depth) {
            bpp = fmt->bits_per_pixel;
            break;
        }
        fmt++;
    }

    /* Set results */
    width = screen_->width_in_pixels;
    height = (screen_->height_in_pixels >> 1) << 1;
    stride = screen_->width_in_pixels * (bpp >> 3);

    shminfo_.shmid = shmget(IPC_PRIVATE, stride * height, (IPC_CREAT | 0666));
    if (shminfo_.shmid == (uint32_t)-1) {
        return false;
    }

    shminfo_.shmaddr = (uint8_t*)shmat(shminfo_.shmid, 0, 0);
    if (shminfo_.shmaddr == (uint8_t*)-1) {
        return false;
    }

    shminfo_.shmseg = xcb_generate_id(connection_);
    xcb_shm_attach(connection_, shminfo_.shmseg, shminfo_.shmid, 0);

    image_.width = width;
    image_.height = height;
    image_.stride = stride;
    image_.data = shminfo_.shmaddr;
    image_.fmt = video::frame::ImageFormat::FMT_0BGR;

    inited_ = true;
    return true;
}

bool XcbScreenGrab::Destroy() {
    if (!inited_) {
        return false;
    }

    xcb_shm_detach(connection_, shminfo_.shmseg);
    shmdt(shminfo_.shmaddr);
    shmctl(shminfo_.shmid, IPC_RMID, 0);
    xcb_disconnect(connection_);

    inited_ = false;
    return true;
}

void XcbScreenGrab::drawMouse() {}

video::frame::Image& XcbScreenGrab::Grab(bool draw_mouse) {
    xcb_generic_error_t* e = nullptr;
    xcb_shm_get_image_cookie_t cookie = xcb_shm_get_image(
            connection_, screen_->root, 0, 0, image_.width, image_.height, ~0,
            XCB_IMAGE_FORMAT_Z_PIXMAP, shminfo_.shmseg, 0);
    xcb_shm_get_image_reply_t* reply =
            xcb_shm_get_image_reply(connection_, cookie, &e);

    if (reply) {
        free(reply);
    }
    xcb_flush(connection_);

    if (draw_mouse) {
        drawMouse();
    }
    return image_;
}
}  // namespace grab
}  // namespace video
