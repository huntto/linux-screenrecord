#include <fcntl.h>
#include <chrono>
#include <iostream>
#include <thread>

#include "audio/faac_encoder.h"
#include "audio/pulse_audio_grab.h"
#include "faac.h"
#include "video/x264_encoder.h"
#include "video/xcb_screen_grab.h"

void RecordAudioTask() {
    const char *audio_file_name = "audio_record.aac";
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
    remove(audio_file_name);
    audio_file = fopen(audio_file_name, "wa+");
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
    fclose(audio_file);
}

void RecordVideoTask() {
    const uint32_t fps_num = 30;
    const uint32_t fps_den = 1;
    auto frame_interval_us = std::chrono::duration<long, std::micro>(
            (long)((double)fps_den / fps_num * 1000000L));
    const char *video_file_name = "screen_record.h264";
    int width, height, stride;

    video::grab::XcbScreenGrab xcb_screen_grab;
    video::encoder::X264Encoder x264_encoder;
    xcb_screen_grab.Init(width, height, stride);
    x264_encoder.Init(width, height, fps_num, fps_den, 9000);

    FILE *video_file;
    remove(video_file_name);
    video_file = fopen(video_file_name, "wa+");

    long frames = 1000;
    while (frames--) {
        auto start_time = std::chrono::system_clock::now();

        video::frame::Image &image = xcb_screen_grab.Grab(false);
        uint8_t *packet = nullptr;
        int length = x264_encoder.Encode(image, &packet);
        if (length > 0) {
            fwrite(packet, length, 1, video_file);
        }
        auto end_time = std::chrono::system_clock::now();
        auto sleeptime = frame_interval_us - (end_time - start_time);
        if (sleeptime.count() > 0) {
            std::this_thread::sleep_for(sleeptime);
        }
    }

    x264_encoder.Destroy();
    xcb_screen_grab.Destroy();
    fclose(video_file);
}

int main(int argc, char const *argv[]) {
    std::thread record_audio_thread(RecordAudioTask);
    std::thread record_video_thread(RecordVideoTask);
    record_audio_thread.join();
    record_video_thread.join();
    return 0;
}
