#include "xcb_screen_grab.h"

#include <stdio.h>
#include <sys/shm.h>
#include <xcb/shape.h>
#include <xcb/shm.h>
#include <xcb/xcb.h>
#include <xcb/xfixes.h>
#include <iostream>

namespace {
video::frame::Image::Format ParseImageFormat(int depth, int bpp) {
    using Format = video::frame::Image::Format;
    Format format = Format::NONE;
    switch (depth) {
        case 32:
            if (bpp == 32) format = Format::FMT_0RGB32;
            break;
        case 24:
            if (bpp == 32)
                format = Format::FMT_0RGB32;
            else if (bpp == 24)
                format = Format::FMT_RGB24;
            break;
        case 16:
            if (bpp == 16) format = Format::FMT_RGB565;
            break;
        case 15:
            if (bpp == 16) format = Format::FMT_RGB555;
            break;
        case 8:
            if (bpp == 8) format = Format::FMT_RGB8;
            break;
    }
    return format;
}

int GetBitsPerPixel(video::frame::Image::Format format) {
    using Format = video::frame::Image::Format;
    switch (format) {
        case Format::FMT_0RGB32:
            return 4;
        case Format::FMT_RGB24:
            return 3;
        case Format::FMT_RGB565:
        case Format::FMT_RGB555:
            return 2;
        case Format::FMT_RGB8:
            return 1;
        default:
            return 0;
    }
}
}  // namespace

namespace video {

namespace frame {

bool ShmImage::Init(int width, int height, Format format) {
    width_ = width;
    height_ = height;
    format_ = format;
    stride_ = width * GetBitsPerPixel(format);

    size_t size = static_cast<size_t>(stride_ * height_);
    int shmid = shmget(IPC_PRIVATE, size, (IPC_CREAT | 0666));
    if (shmid == -1) {
        return false;
    }

    void *shmaddr = shmat(shmid, nullptr, 0);
    if (shmaddr == (void *)-1) {
        return false;
    }

    shmid_ = shmid;
    shmaddr_ = shmaddr;
    return true;
}

ShmImage::~ShmImage() {
    if (shmaddr_ != nullptr) {
        shmdt(shmaddr_);
        shmaddr_ = nullptr;
    }
    if (shmid_ != -1) {
        shmctl(shmid_, IPC_RMID, nullptr);
        shmid_ = -1;
    }
}

}  // namespace frame

namespace grab {

XcbScreenGrab::XcbScreenGrab() {}

bool XcbScreenGrab::Init() {
    if (connection_ != nullptr) {
        return true;
    }

    /* Open the connection to the X server */
    connection_ = xcb_connect(nullptr, nullptr);

    /* Get the first screen */
    const xcb_setup_t *setup = xcb_get_setup(connection_);
    xcb_screen_iterator_t iter = xcb_setup_roots_iterator(setup);
    screen_ = iter.data;

    /* Get bpp */
    const xcb_format_t *fmt = xcb_setup_pixmap_formats(setup);
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
    width_ = screen_->width_in_pixels;
    height_ = (screen_->height_in_pixels >> 1) << 1;
    stride_ = screen_->width_in_pixels * (bpp >> 3);
    format_ = ParseImageFormat(screen_->root_depth, bpp);

    /* Generate shmseg */
    shmseg_ = xcb_generate_id(connection_);

    return true;
}

bool XcbScreenGrab::Destroy() {
    if (connection_ != nullptr) {
        xcb_disconnect(connection_);
        connection_ = nullptr;
        return true;
    }
    return false;
}

void XcbScreenGrab::drawMouse() {}

bool XcbScreenGrab::Grab(std::shared_ptr<frame::Image> image, bool draw_mouse) {
    if (connection_ == nullptr) {
        return false;
    }

    frame::ShmImage *shm_img = dynamic_cast<frame::ShmImage *>(image.get());
    if (shm_img == nullptr || shm_img->GetFormat() != format_) {
        return false;
    }

    xcb_shm_attach(connection_, shmseg_,
                   static_cast<uint32_t>(shm_img->GetShmId()), 0);

    xcb_generic_error_t *e = nullptr;
    xcb_shm_get_image_cookie_t cookie = xcb_shm_get_image(
            connection_, screen_->root, 0, 0,
            static_cast<uint16_t>(shm_img->GetWidth()),
            static_cast<uint16_t>(shm_img->GetHeight()),
            static_cast<uint32_t>(~0), XCB_IMAGE_FORMAT_Z_PIXMAP, shmseg_, 0);

    xcb_shm_get_image_reply_t *reply =
            xcb_shm_get_image_reply(connection_, cookie, &e);

    if (reply) {
        free(reply);
    }

    if (e) {
        std::cerr << "Cannot get the image data from xcb server!\n"
                  << std::endl;
        return false;
    }

    xcb_shm_detach(connection_, shmseg_);
    xcb_flush(connection_);

    if (draw_mouse) {
        drawMouse();
    }
    return true;
}

}  // namespace grab
}  // namespace video
