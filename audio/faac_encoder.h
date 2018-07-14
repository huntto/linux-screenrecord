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

    virtual bool Init(SampleFormat sample_format, unsigned long sample_rate,
                      unsigned int channels) override;
    virtual void Release() override;
    virtual int Encode(std::vector<uint8_t>& input_buffer,
                       std::vector<uint8_t>& output_buffer) override;

    virtual std::vector<uint8_t> GetInputBuffer() override {
        return std::vector<uint8_t>(input_buffer_size_);
    }
    virtual std::vector<uint8_t> GetOutputBuffer() override {
        return std::vector<uint8_t>(output_buffer_size_);
    }

    virtual ~FaacEncoder() override = default;

private:
    faacEncHandle faac_enc_handle_;
    unsigned long input_buffer_size_ = 1024;
    unsigned long output_buffer_size_ = 2048;
    unsigned long input_samples_;
};

}  // namespace encoder
}  // namespace audio

#endif  // AUDIO_ENCODER_H_
