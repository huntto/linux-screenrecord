#ifndef FAAC_ENCODER_H_
#define FAAC_ENCODER_H_

#include "audio_encoder.h"

#include "faac.h"

namespace audio {
namespace encoder {

class FaacEncoder : public AudioEncoder {
public:
    FaacEncoder() {}
    FaacEncoder(FaacEncoder&) = delete;
    FaacEncoder& operator=(FaacEncoder&) = delete;

    virtual bool Init(unsigned long sampleRate,
                      unsigned int numChannels) override;
    virtual void Release() override;
    virtual int Encode(std::vector<uint8_t>& input_buffer,
                       unsigned int input_sapmles,
                       std::vector<uint8_t>& output_buffer) override;
    virtual ~FaacEncoder() override = default;

private:
    faacEncHandle faac_enc_handle_;
};

}  // namespace encoder
}  // namespace audio

#endif  // AUDIO_ENCODER_H_
