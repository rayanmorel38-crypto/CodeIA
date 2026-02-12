#pragma once
/**
 * @class LowLatencyAudioTask
 * @brief Traitement audio basse-latence (DSP optimisé).
 */
#include <vector>
#include <string>
#include <stdexcept>

class LowLatencyAudioTask {
public:
    LowLatencyAudioTask();
    ~LowLatencyAudioTask();
    void configure(int sampleRate, int channels);
    std::vector<float> run(const std::vector<float>& input);
    void log(const std::string& message) const;
private:
    int sampleRate_;
    int channels_;
    bool configured_;
};
