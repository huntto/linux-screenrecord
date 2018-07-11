#include "pulse_audio_grab.h"

#include <iostream>

namespace {

enum class State { NONE, CONNECTING, CONNECTED, ERROR };

struct Device {
    enum class Type { SOURCE, SINK };

    std::string name;
    std::string description;
    Type type;
    pa_sample_spec sample_spec;
};

void StateCallback(pa_context *context, void *userdata) {
    State *state = static_cast<State *>(userdata);
    switch (pa_context_get_state(context)) {
        case PA_CONTEXT_READY:
            *state = State::CONNECTED;
            break;
        case PA_CONTEXT_TERMINATED:
        case PA_CONTEXT_FAILED:
            *state = State::ERROR;
            break;
        default:
            break;
    }
}

bool Connect(pa_mainloop *mainloop, pa_context *context, State &state) {
    state = State::CONNECTING;
    pa_context_set_state_callback(context, StateCallback, &state);

    if (pa_context_connect(context, nullptr, PA_CONTEXT_NOFLAGS, nullptr) < 0) {
        std::cerr << "CONNECTION::Can not connect to default server!"
                  << std::endl;
        return false;
    }

    int retval(0);
    while (state == State::CONNECTING) {
        if (pa_mainloop_iterate(mainloop, 1, &retval) < 0) {
            std::cerr << "CONNECTION::mainloop iterate error:" << retval
                      << std::endl;
            return false;
        }
    }

    if (state == State::ERROR) {
        std::cerr << "CONNECTION::connect to default server failed."
                  << std::endl;
        return false;
    }
    return true;
}

void Disconnect(pa_mainloop *mainloop, pa_context *context, State &state) {
    if (context != nullptr) {
        pa_context_set_state_callback(context, nullptr, nullptr);
        if (state == State::CONNECTED) pa_context_disconnect(context);
        pa_context_unref(context);
    }
    if (mainloop != nullptr) {
        pa_mainloop_free(mainloop);
    }
}

void SourceListCallback(pa_context *, const pa_source_info *info, int eol,
                        void *userdata) {
    if (eol != 0) return;
    std::list<Device> *source_list = static_cast<std::list<Device> *>(userdata);
    if (info != nullptr && info->monitor_of_sink != PA_INVALID_INDEX) {
        Device device = {info->name, info->description, Device::Type::SOURCE,
                         info->sample_spec};
        source_list->push_back(device);
    }
}

void GetSourceDevices(std::string name, std::list<Device> &source_list) {
    pa_mainloop *mainloop = pa_mainloop_new();
    pa_mainloop_api *mainloop_api = pa_mainloop_get_api(mainloop);
    pa_context *context = pa_context_new(mainloop_api, name.c_str());

    State state{State::NONE};
    if (!Connect(mainloop, context, state)) {
        Disconnect(mainloop, context, state);
        return;
    }

    pa_operation *operation = pa_context_get_source_info_list(
            context, SourceListCallback, &source_list);

    int retval{0};
    while (pa_operation_get_state(operation) == PA_OPERATION_RUNNING) {
        pa_mainloop_iterate(mainloop, 1, &retval);
    }
    pa_operation_unref(operation);

    Disconnect(mainloop, context, state);
}

}  // namespace

namespace audio {
namespace grab {

PulseAudioGrab::PulseAudioGrab(std::string name)
        : name_(name), inited_(false), simple_reader_(nullptr) {}

bool PulseAudioGrab::Init(uint32_t sample_rate, uint8_t channels) {
    if (inited_) {
        return true;
    }

    std::list<Device> source_devices{};
    GetSourceDevices(name_, source_devices);

    if (source_devices.size() < 1) {
        return false;
    }

    const char *source_name = source_devices.begin()->name.c_str();
    const pa_sample_spec sample_spec = {PA_SAMPLE_FLOAT32LE, sample_rate,
                                        channels};

    int error{0};
    if (!(simple_reader_ = pa_simple_new(
                  nullptr, name_.c_str(), PA_STREAM_RECORD, source_name,
                  name_.c_str(), &sample_spec, nullptr, nullptr, &error))) {
        fprintf(stderr, __FILE__ ": pa_simple_new() failed: %s\n",
                pa_strerror(error));
        Release();
        return false;
    }

    inited_ = true;
    return true;
}

void PulseAudioGrab::Release() {
    if (simple_reader_ != nullptr) {
        pa_simple_free(simple_reader_);
        simple_reader_ = nullptr;
    }
    inited_ = false;
}

bool PulseAudioGrab::Grab(std::vector<uint8_t> &buffer) {
    if (!inited_) {
        return false;
    }

    int error{0};
    if (pa_simple_read(simple_reader_, buffer.data(), buffer.size(), &error) <
        0) {
        fprintf(stderr, __FILE__ ": pa_simple_read() failed: %s\n",
                pa_strerror(error));
        Release();
        return false;
    }

    return true;
}

}  // namespace grab
}  // namespace audio
