// speech_recognizer.h
#pragma once

#include <string>
#include <vector>

namespace cpp_engine::modules::audio {

class SpeechRecognizer {
public:
    SpeechRecognizer();
    ~SpeechRecognizer();

    // Initialize recognizer (optionally provide model path)
    bool initialize(const std::string &model_path="");

    // Feed raw PCM data and return recognized text (blocking)
    std::string recognize(const std::vector<int16_t> &pcm_samples, int sample_rate=16000);

    // Reset internal state
    void reset();
};

} // namespace
