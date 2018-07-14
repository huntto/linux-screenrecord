#ifndef SAMPLE_FORMAT_H_
#define SAMPLE_FORMAT_H_

namespace audio {
enum class SampleFormat {
    SAMPLE_FMT_S16LE,
    SAMPLE_FMT_S16BE,
    SAMPLE_FMT_FLOAT32LE,
    SAMPLE_FMT_FLOAT32BE,
    SAMPLE_FMT_S24LE,
    SAMPLE_FMT_S24BE,
    SAMPLE_FMT_S24_32LE,
    SAMPLE_FMT_S24_32BE
};
}  // namespace audio

#endif  // SAMPLE_FORMAT_H_
