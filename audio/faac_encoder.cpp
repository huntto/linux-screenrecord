#include "faac_encoder.h"

#include <iostream>

namespace audio {
namespace encoder {

bool FaacEncoder::Init(unsigned long sample_rate, unsigned int num_channels) {
    unsigned long input_samples{0};
    unsigned long max_output_bytes{0};
    faac_enc_handle_ = faacEncOpen(sample_rate, num_channels, &input_samples,
                                   &max_output_bytes);
    if (faac_enc_handle_ == nullptr) {
        std::cerr << "open faac enc failed." << std::endl;
        return false;
    }
    std::cout << "input_samples:" << input_samples
              << " max_output_bytes:" << max_output_bytes << std::endl;

    faacEncConfigurationPtr configuration_ptr =
            faacEncGetCurrentConfiguration(faac_enc_handle_);
    configuration_ptr->inputFormat = FAAC_INPUT_32BIT;
    configuration_ptr->outputFormat = 1;
    configuration_ptr->aacObjectType = LOW;
    configuration_ptr->useLfe = 0;
    configuration_ptr->bitRate = 48000;
    configuration_ptr->bandWidth = 32000;
    faacEncSetConfiguration(faac_enc_handle_, configuration_ptr);

    return true;
}

void FaacEncoder::Release() {}

int FaacEncoder::Encode(std::vector<uint8_t>& input_buffer,
                        unsigned int input_sapmles,
                        std::vector<uint8_t>& output_buffer) {
    if (faac_enc_handle_ == nullptr) {
        return 0;
    }
    int ret = faacEncEncode(
            faac_enc_handle_, reinterpret_cast<int32_t*>(input_buffer.data()),
            input_sapmles,
            reinterpret_cast<unsigned char*>(output_buffer.data()),
            static_cast<unsigned int>(output_buffer.size()));

    return ret;
}

}  // namespace encoder
}  // namespace audio
