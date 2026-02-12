// speech_synthesizer.cpp -- lightweight stub implementation
#include "modules/audio/speech_synthesizer.h"
#include <iostream>

namespace cpp_engine::modules::audio {

SpeechSynthesizer::SpeechSynthesizer() {}
SpeechSynthesizer::~SpeechSynthesizer() {}

bool SpeechSynthesizer::initialize(const std::string &voice) {
    (void)voice;
    return true;
}

std::vector<int16_t> SpeechSynthesizer::synthesize(const std::string &text, int sample_rate) {
    (void)text; (void)sample_rate;
    // Return an empty vector (stub)
    return {};
}

void SpeechSynthesizer::shutdown() {}

} // namespace

extern "C" void register_module() {
    std::cout << "[SpeechSynthesizer] registered" << std::endl;
}
