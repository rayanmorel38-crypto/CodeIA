// Tâche de génération de son (implémentation robuste)
#include "../../include/tasks/audio_generation_task.h"
#include "utils/logger.h"
#include <fstream>
#include <cstdint>
#include <cmath>

AudioGenerationTask::AudioGenerationTask()
    : sampleRate_(48000), channels_(1), configured_(false) {}

AudioGenerationTask::~AudioGenerationTask() {}

void AudioGenerationTask::configure(const std::string& modelPath, int sampleRate, int channels) {
    modelPath_ = modelPath;
    sampleRate_ = sampleRate;
    channels_ = channels;
    configured_ = true;
    log("AudioGenerationTask configured: model=" + modelPath_ + ", sr=" + std::to_string(sampleRate_) + ", ch=" + std::to_string(channels_));
}

std::vector<float> AudioGenerationTask::run(const std::string& prompt) {
    if (!configured_) throw std::runtime_error("AudioGenerationTask non configurée");
    log("Génération audio pour prompt: " + prompt);
    // Simulation simple: génère une courte sinusoïde dépendant du prompt hash
    size_t h = 0;
    for (char c : prompt) h = h * 31 + static_cast<unsigned char>(c);
    float freq = 220.0f + static_cast<float>(h % 800);
    int duration_s = 2; // 2 secondes par défaut
    int totalSamples = duration_s * sampleRate_ * channels_;
    std::vector<float> out; out.reserve(totalSamples);
    for (int i = 0; i < totalSamples; ++i) {
        float t = static_cast<float>(i) / static_cast<float>(sampleRate_);
        float s = 0.1f * std::sin(2.0f * 3.14159265f * freq * t);
        out.push_back(s);
    }
    log("Génération terminée, samples=" + std::to_string((int)out.size()));
    return out;
}

bool AudioGenerationTask::saveToWav(const std::string& outPath, const std::vector<float>& pcm, int sampleRate, int channels) {
    std::ofstream ofs(outPath, std::ios::binary);
    if (!ofs) return false;
    // Header WAV basique 16-bit PCM
    int32_t subchunk1Size = 16;
    int16_t audioFormat = 1; // PCM
    int16_t numChannels = static_cast<int16_t>(channels);
    int32_t byteRate = sampleRate * numChannels * 2;
    int16_t blockAlign = numChannels * 2;
    int32_t bitsPerSample = 16;
    int32_t dataSize = static_cast<int32_t>(pcm.size()) * 2;

    // RIFF header
    ofs.write("RIFF", 4);
    int32_t chunkSize = 36 + dataSize;
    ofs.write(reinterpret_cast<const char*>(&chunkSize), 4);
    ofs.write("WAVE", 4);

    // fmt subchunk
    ofs.write("fmt ", 4);
    ofs.write(reinterpret_cast<const char*>(&subchunk1Size), 4);
    ofs.write(reinterpret_cast<const char*>(&audioFormat), 2);
    ofs.write(reinterpret_cast<const char*>(&numChannels), 2);
    ofs.write(reinterpret_cast<const char*>(&sampleRate), 4);
    ofs.write(reinterpret_cast<const char*>(&byteRate), 4);
    ofs.write(reinterpret_cast<const char*>(&blockAlign), 2);
    ofs.write(reinterpret_cast<const char*>(&bitsPerSample), 2);

    // data subchunk
    ofs.write("data", 4);
    ofs.write(reinterpret_cast<const char*>(&dataSize), 4);

    // Write samples (float -> int16)
    for (size_t i = 0; i < pcm.size(); ++i) {
        float v = pcm[i];
        if (v > 1.0f) v = 1.0f; if (v < -1.0f) v = -1.0f;
        int16_t s = static_cast<int16_t>(std::lround(v * 32767.0f));
        ofs.write(reinterpret_cast<const char*>(&s), sizeof(s));
    }
    return true;
}

void AudioGenerationTask::log(const std::string& message) const {
    cpp_engine::utils::Logger::instance().info(std::string("[AudioGenerationTask] ") + message);
}
