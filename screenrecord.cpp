#include <fcntl.h>
#include <iostream>
#include <sys/time.h>
#include <sys/unistd.h>

#include "src/x264_encoder.h"
#include "src/xcb_screengrab.h"

const uint32_t kFpsNum          = 30;
const uint32_t kFpsDen          = 1;
const long     kFrameIntervalUs = (long)((double)kFpsDen / kFpsNum * 1000000L);
const char*    kFileName        = "screen_record.h264";

static long timestamp() {
    struct timeval  tv;
    struct timezone tz;
    gettimeofday(&tv, &tz);
    return tv.tv_sec * 1000000L + tv.tv_usec;
}

int main(int argc, char const* argv[]) {
    uint32_t                    width, height, stride;
    video::grab::XcbScreenGrab  xcb_screen_grab;
    video::encoder::X264Encoder x264_encoder;
    xcb_screen_grab.Init(width, height, stride);
    x264_encoder.Init(width, height, kFpsNum, kFpsDen, 9000);

    std::cout << "width:" << width << " height:" << height << " stride:" << stride << std::endl;
    FILE* file;
    remove(kFileName);
    file = fopen(kFileName, "wa+");

    long frames = 1000;
    while (frames--) {
        long frame_timestamp = timestamp();

        video::frame::Image& image  = xcb_screen_grab.Grab(false);
        uint8_t*             packet = nullptr;
        int                  length = x264_encoder.Encode(image, &packet);
        if (length > 0) {
            fwrite(packet, length, 1, file);
        }
        long sleeptime = kFrameIntervalUs - (timestamp() - frame_timestamp);
        if (sleeptime > 0) {
            usleep(sleeptime);
        }
    }

    x264_encoder.Destroy();
    xcb_screen_grab.Destroy();
    fclose(file);
    return 0;
}
