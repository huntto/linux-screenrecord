#include <fcntl.h>
#include <sys/time.h>
#include <unistd.h>
#include <iostream>

#include "audio/faac_encoder.h"
#include "audio/pulse_audio_grab.h"
#include "faac.h"
#include "video/x264_encoder.h"
#include "video/xcb_screen_grab.h"

const uint32_t kFpsNum = 30;
const uint32_t kFpsDen = 1;
const long kFrameIntervalUs = (long)((double)kFpsDen / kFpsNum * 1000000L);
const char *kFileName = "screen_record.h264";
const char *kAudioFileName = "audio_record.aac";

static long timestamp() {
    struct timeval tv;
    struct timezone tz;
    gettimeofday(&tv, &tz);
    return tv.tv_sec * 1000000L + tv.tv_usec;
}

int main(int argc, char const *argv[]) {
    int width, height, stride;
    std::string client_name = "recorder";
    audio::grab::PulseAudioGrab pulse_audio_grab(client_name);

    uint32_t sample_rate = 44100;
    uint8_t channels = 2;
    pulse_audio_grab.Init(audio::SampleFormat::SAMPLE_FMT_S16LE, sample_rate,
                          channels);

    audio::encoder::FaacEncoder faac_encoder;
    faac_encoder.Init(audio::SampleFormat::SAMPLE_FMT_S16LE, sample_rate,
                      channels);
    FILE *audio_file;
    remove(kAudioFileName);
    audio_file = fopen(kAudioFileName, "wa+");
    long samples = 1000;

    std::vector<uint8_t> input_buffer = faac_encoder.GetInputBuffer();
    std::vector<uint8_t> output_buffer = faac_encoder.GetOutputBuffer();

    while (samples--) {
        pulse_audio_grab.Grab(input_buffer);
        int length = faac_encoder.Encode(input_buffer, output_buffer);
        if (length > 0) {
            fwrite(output_buffer.data(), length, 1, audio_file);
        }
    }
    pulse_audio_grab.Release();
    faac_encoder.Release();

    //    video::grab::XcbScreenGrab xcb_screen_grab;
    //    video::encoder::X264Encoder x264_encoder;
    //    xcb_screen_grab.Init(width, height, stride);
    //    x264_encoder.Init(width, height, kFpsNum, kFpsDen, 9000);

    //    std::cout << "width:" << width << " height:" << height
    //              << " stride:" << stride << std::endl;
    //    FILE *file;
    //    remove(kFileName);
    //    file = fopen(kFileName, "wa+");

    //    long frames = 1000;
    //    while (frames--) {
    //        long frame_timestamp = timestamp();

    //        video::frame::Image &image = xcb_screen_grab.Grab(false);
    //        uint8_t *packet = nullptr;
    //        int length = x264_encoder.Encode(image, &packet);
    //        if (length > 0) {
    //            fwrite(packet, length, 1, file);
    //        }
    //        long sleeptime = kFrameIntervalUs - (timestamp() -
    //        frame_timestamp); if (sleeptime > 0) {
    //            usleep(sleeptime);
    //        }
    //    }

    //    x264_encoder.Destroy();
    //    xcb_screen_grab.Destroy();
    //    fclose(file);
    return 0;
}
