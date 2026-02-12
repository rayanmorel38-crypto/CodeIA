// audio_module.cpp
#include "modules/audio/audio_module.h"
#include <iostream>

namespace cpp_engine::modules::audio {

AudioModule::AudioModule() {}
AudioModule::~AudioModule() {}

bool AudioModule::start() { return true; }
void AudioModule::stop() {}

bool AudioModule::has_tts() const { return true; }
bool AudioModule::has_asr() const { return true; }

} // namespace

extern "C" void register_module() {
    std::cout << "[AudioModule] registered" << std::endl;
}
