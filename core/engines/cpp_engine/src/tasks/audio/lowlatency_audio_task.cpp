#include "../../include/tasks/lowlatency_audio_task.h"
#include <iostream>
#include "utils/logger.h"

LowLatencyAudioTask::LowLatencyAudioTask() : sampleRate_(48000), channels_(2), configured_(false) {}
LowLatencyAudioTask::~LowLatencyAudioTask() {}

void LowLatencyAudioTask::configure(int sampleRate, int channels) {
    sampleRate_ = sampleRate; channels_ = channels; configured_ = true;
    log("Audio configured: " + std::to_string(sampleRate_) + "Hz, ch=" + std::to_string(channels_));
}

std::vector<float> LowLatencyAudioTask::run(const std::vector<float>& input) {
    if (!configured_) throw std::runtime_error("LowLatencyAudioTask non configurée");
    log("Traitement audio basse-latence (simulation)");
    // Simulation: passthrough
    return input;
}

void LowLatencyAudioTask::log(const std::string& message) const {
    cpp_engine::utils::Logger::instance().info("[LowLatencyAudioTask] " + message);
}
