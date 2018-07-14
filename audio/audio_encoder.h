#ifndef AUDIO_ENCODER_H_
#define AUDIO_ENCODER_H_

#include <stdint.h>
#include <vector>

#include "sample_format.h"

namespace audio {
namespace encoder {

class AudioEncoder {
public:
    virtual bool Init(SampleFormat sample_format, unsigned long sample_rate,
                      unsigned int channels) = 0;
    virtual void Release() = 0;
    virtual int Encode(std::vector<uint8_t>& input_buffer,
                       std::vector<uint8_t>& output_buffer) = 0;

    virtual std::vector<uint8_t> GetInputBuffer() = 0;
    virtual std::vector<uint8_t> GetOutputBuffer() = 0;

    virtual ~AudioEncoder() = default;
};

}  // namespace encoder
}  // namespace audio

#endif  // AUDIO_ENCODER_H_
