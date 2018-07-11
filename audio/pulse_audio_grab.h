#ifndef PULSE_AUDIO_GRAB_H_
#define PULSE_AUDIO_GRAB_H_

#include "audio_grab.h"

#include <pulse/pulseaudio.h>
#include <pulse/simple.h>
#include <list>
#include <string>

namespace audio {
namespace grab {

class PulseAudioGrab : public AudioGrab {
public:
    PulseAudioGrab(std::string name);
    PulseAudioGrab(PulseAudioGrab&) = delete;
    PulseAudioGrab& operator=(PulseAudioGrab&) = delete;

    virtual ~PulseAudioGrab() override = default;

    virtual bool Init(uint32_t sample_rate, uint8_t channels) override;
    virtual void Release() override;

    virtual bool Grab(std::vector<uint8_t>& buffer) override;

private:
    std::string name_;
    bool inited_;
    pa_simple* simple_reader_;
};

}  // namespace grab
}  // namespace audio
#endif  // PULSE_AUDIO_GRAB_H_
