#include "faac_encoder.h"

#include <iostream>

namespace {
unsigned int GetInputFormat(audio::SampleFormat sample_format) {
    using SampleFormat = audio::SampleFormat;
    switch (sample_format) {
        case SampleFormat::SAMPLE_FMT_S16LE:
        case SampleFormat::SAMPLE_FMT_S16BE:
            return FAAC_INPUT_16BIT;
        case SampleFormat::SAMPLE_FMT_FLOAT32LE:
        case SampleFormat::SAMPLE_FMT_FLOAT32BE:
            return FAAC_INPUT_FLOAT;
        case SampleFormat::SAMPLE_FMT_S24LE:
        case SampleFormat::SAMPLE_FMT_S24BE:
            return FAAC_INPUT_24BIT;
        case SampleFormat::SAMPLE_FMT_S24_32LE:
        case SampleFormat::SAMPLE_FMT_S24_32BE:
            return FAAC_INPUT_32BIT;
        default:
            return FAAC_INPUT_NULL;
    }
}

unsigned int GetPcmBitSize(unsigned int input_format) {
    switch (input_format) {
        case FAAC_INPUT_16BIT:
            return 16;
        case FAAC_INPUT_24BIT:
            return 24;
        case FAAC_INPUT_32BIT:
            return 32;
        case FAAC_INPUT_FLOAT:
            return 32;
        case FAAC_INPUT_NULL:
        default:
            return 0;
    }
}

}  // namespace

namespace audio {
namespace encoder {

bool FaacEncoder::Init(SampleFormat sample_format, unsigned long sample_rate,
                       unsigned int channels) {
    if (faac_enc_handle_ != nullptr) {
        Release();
    }

    faac_enc_handle_ = faacEncOpen(sample_rate, channels, &input_samples_,
                                   &output_buffer_size_);
    if (faac_enc_handle_ == nullptr) {
        std::cerr << "open faac enc failed." << std::endl;
        return false;
    }

    unsigned int input_format = GetInputFormat(sample_format);
    input_buffer_size_ = input_samples_ * GetPcmBitSize(input_format) / 8;

    faacEncConfigurationPtr configuration_ptr =
            faacEncGetCurrentConfiguration(faac_enc_handle_);
    configuration_ptr->inputFormat = input_format;
    configuration_ptr->outputFormat = 1;
    configuration_ptr->aacObjectType = LOW;
    configuration_ptr->useLfe = 0;
    configuration_ptr->bitRate = 48000;
    configuration_ptr->bandWidth = 32000;
    faacEncSetConfiguration(faac_enc_handle_, configuration_ptr);

    return true;
}

void FaacEncoder::Release() {
    if (faac_enc_handle_ != nullptr) {
        faacEncClose(faac_enc_handle_);
        faac_enc_handle_ = nullptr;
    }
}

int FaacEncoder::Encode(std::vector<uint8_t>& input_buffer,
                        std::vector<uint8_t>& output_buffer) {
    if (faac_enc_handle_ == nullptr) {
        return 0;
    }

    int ret = faacEncEncode(
            faac_enc_handle_, reinterpret_cast<int32_t*>(input_buffer.data()),
            input_samples_,
            reinterpret_cast<unsigned char*>(output_buffer.data()),
            static_cast<unsigned int>(output_buffer.size()));

    return ret;
}

}  // namespace encoder
}  // namespace audio
