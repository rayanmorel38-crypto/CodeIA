#pragma once
#include <string>
#include <vector>
#include <stdexcept>

/**
 * @class SpeechSynthesisTask
 * @brief Synthèse vocale (TTS) - renvoie PCM float.
 */
class SpeechSynthesisTask {
public:
    SpeechSynthesisTask();
    ~SpeechSynthesisTask();
    void configure(const std::string& modelPath = "", int sampleRate = 48000);
    std::vector<float> run(const std::string& text);
    static bool saveToWav(const std::string& outPath, const std::vector<float>& pcm, int sampleRate, int channels);
    void log(const std::string& message) const;
private:
    std::string modelPath_;
    int sampleRate_;
    bool configured_;
};
