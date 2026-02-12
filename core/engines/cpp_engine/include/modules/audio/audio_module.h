// audio_module.h
#pragma once

#include <string>

namespace cpp_engine::modules::audio {

class AudioModule {
public:
    AudioModule();
    ~AudioModule();

    bool start();
    void stop();

    // Convenience accessors
    bool has_tts() const;
    bool has_asr() const;
};

} // namespace
