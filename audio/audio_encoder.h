#ifndef AUDIO_ENCODER_H_
#define AUDIO_ENCODER_H_

#include <stdint.h>
#include <vector>

namespace audio {
namespace encoder {

class AudioEncoder {
public:
    virtual bool Init(unsigned long sampleRate, unsigned int numChannels) = 0;
    virtual void Release() = 0;
    virtual int Encode(std::vector<uint8_t>& input_buffer,
                       unsigned int input_sapmles,
                       std::vector<uint8_t>& output_buffer) = 0;
    virtual ~AudioEncoder() = default;
};

}  // namespace encoder
}  // namespace audio

#endif  // AUDIO_ENCODER_H_
