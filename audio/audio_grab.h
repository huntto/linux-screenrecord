#ifndef AUDIO_GRAB_H_
#define AUDIO_GRAB_H_

#include <stdint.h>
#include <vector>

namespace audio {
namespace grab {

class AudioGrab {
public:
    virtual ~AudioGrab() = default;

    virtual bool Init(uint32_t sample_rate, uint8_t channels) = 0;
    virtual void Release() = 0;

    virtual bool Grab(std::vector<uint8_t>& buffer) = 0;
};
}  // namespace grab
}  // namespace audio

#endif  // AUDIO_GRAB_H_
