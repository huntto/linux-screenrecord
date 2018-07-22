#include <fcntl.h>
#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

#include "audio/faac_encoder.h"
#include "audio/pulse_audio_grab.h"
#include "avformat/flv_encoder.h"
#include "video/x264_encoder.h"
#include "video/xcb_screen_grab.h"

static const uint32_t kFpsNum = 30;
static const uint32_t kFpsDen = 1;
static const char *kFileName = "screenrecord.flv";
static const int kWidth = 1280;
static const int kHeight = 720;
static bool recording = true;

void RecordAudioTask(
        std::shared_ptr<avformat::encoder::AVFormatEncoder> avformat_enc) {
    std::string client_name = "recorder";
    audio::grab::PulseAudioGrab pulse_audio_grab(client_name);

    uint32_t sample_rate = 44100;
    uint8_t channels = 2;
    pulse_audio_grab.Init(audio::SampleFormat::SAMPLE_FMT_S16LE, sample_rate,
                          channels);

    audio::encoder::FaacEncoder faac_encoder;
    faac_encoder.Init(audio::SampleFormat::SAMPLE_FMT_S16LE, sample_rate,
                      channels);

    std::vector<uint8_t> input_buffer = faac_encoder.GetInputBuffer();
    std::vector<uint8_t> output_buffer = faac_encoder.GetOutputBuffer();

    auto begin_time = std::chrono::system_clock::now();
    while (recording) {
        auto start_time = std::chrono::system_clock::now();
        unsigned int time_stamp = static_cast<unsigned int>(
                std::chrono::duration_cast<std::chrono::milliseconds>(
                        start_time - begin_time)
                        .count());
        pulse_audio_grab.Grab(input_buffer);
        int length = faac_encoder.Encode(input_buffer, output_buffer);
        if (length > 0) {
            avformat_enc->WriteAudioPacket(output_buffer.data() + 7, length - 7,
                                           time_stamp);
        }
    }
    pulse_audio_grab.Release();
    faac_encoder.Release();
}

void RecordVideoTask(
        std::shared_ptr<avformat::encoder::AVFormatEncoder> avformat_enc,
        const int width, const int height, const uint32_t fps_num,
        const uint32_t fps_den) {
    auto frame_interval_us = std::chrono::duration<long, std::micro>(
            static_cast<long>(1000000.0 * fps_den / fps_num));

    using namespace video::frame;
    using namespace video::grab;

    video::grab::XcbScreenGrab xcb_screen_grab;
    video::encoder::X264Encoder x264_encoder;
    xcb_screen_grab.Init();
    Image::Format format = xcb_screen_grab.GetFormat();

    x264_encoder.Init(width, height, fps_num, fps_den, 9000);

    std::shared_ptr<Image> image = std::shared_ptr<Image>(new ShmImage);
    image->Init(width, height, format);

    auto begin_time = std::chrono::system_clock::now();
    while (recording) {
        auto start_time = std::chrono::system_clock::now();

        xcb_screen_grab.Grab(image, false);

        uint8_t *packet = nullptr;
        int is_keyframe = 0;
        int length = x264_encoder.Encode(image, &packet, is_keyframe);

        unsigned int timestamp = static_cast<unsigned int>(
                std::chrono::duration_cast<std::chrono::milliseconds>(
                        start_time - begin_time)
                        .count());
        if (length > 0) {
            avformat_enc->WriteVideoPacket(packet, length, timestamp,
                                           is_keyframe);
        }
        auto end_time = std::chrono::system_clock::now();
        auto sleeptime = frame_interval_us - (end_time - start_time);
        if (sleeptime.count() > 0) {
            std::this_thread::sleep_for(sleeptime);
        }
    }

    x264_encoder.Destroy();
    xcb_screen_grab.Destroy();
}

int main(int argc, char const *argv[]) {
    using namespace avformat::encoder;
    std::shared_ptr<AVFormatEncoder> avformat_enc =
            std::shared_ptr<AVFormatEncoder>(new FlvEncoder());
    avformat_enc->Init(kFileName, kFpsNum / kFpsDen, kWidth, kHeight);

    std::thread record_audio_thread(RecordAudioTask, avformat_enc);
    std::thread record_video_thread(RecordVideoTask, avformat_enc, kWidth,
                                    kHeight, kFpsNum, kFpsDen);

    std::this_thread::sleep_for(std::chrono::minutes(1));

    recording = false;
    record_audio_thread.join();
    record_video_thread.join();
    return 0;
}
