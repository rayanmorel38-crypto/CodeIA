// speech_synthesizer.h
#pragma once

#include <string>
#include <vector>

namespace cpp_engine::modules::audio {

class SpeechSynthesizer {
public:
    SpeechSynthesizer();
    ~SpeechSynthesizer();

    // Initialize TTS engine (voice optional)
    bool initialize(const std::string &voice = "default");

    // Synthesize text to PCM (mono 16-bit, sample_rate)
    // Returns raw PCM bytes
    std::vector<int16_t> synthesize(const std::string &text, int sample_rate = 22050);

    void shutdown();
};

} // namespace
