// speech_recognizer.cpp -- lightweight stub implementation
#include "modules/audio/speech_recognizer.h"
#include <iostream>

namespace cpp_engine::modules::audio {

SpeechRecognizer::SpeechRecognizer() {}
SpeechRecognizer::~SpeechRecognizer() {}

bool SpeechRecognizer::initialize(const std::string &model_path) {
    (void)model_path;
    return true;
}

std::string SpeechRecognizer::recognize(const std::vector<int16_t> &pcm_samples, int sample_rate) {
    (void)pcm_samples; (void)sample_rate;
    return std::string("<recognized-speech-stub>");
}

void SpeechRecognizer::reset() {}

} // namespace

extern "C" void register_module() {
    std::cout << "[SpeechRecognizer] registered" << std::endl;
}
