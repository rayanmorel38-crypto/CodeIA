#include "../../include/tasks/speech_synthesis_task.h"
#include <iostream>
#include <fstream>
#include <cmath>

SpeechSynthesisTask::SpeechSynthesisTask() : sampleRate_(48000), configured_(false) {}
SpeechSynthesisTask::~SpeechSynthesisTask() {}

void SpeechSynthesisTask::configure(const std::string& modelPath, int sampleRate) {
    modelPath_ = modelPath; sampleRate_ = sampleRate; configured_ = true; log("Model: " + modelPath_);
}

std::vector<float> SpeechSynthesisTask::run(const std::string& text) {
    if (!configured_) throw std::runtime_error("SpeechSynthesisTask non configurée");
    log("Synthesizing: " + text);
    int dur = 2; int total = dur * sampleRate_;
    std::vector<float> pcm(total, 0.0f);
    float freq = 220.0f;
    for (int i = 0; i < total; ++i) pcm[i] = 0.05f * std::sin(2.0f * 3.14159265f * freq * (float)i / (float)sampleRate_);
    return pcm;
}

bool SpeechSynthesisTask::saveToWav(const std::string& outPath, const std::vector<float>& pcm, int sampleRate, int channels) {
    // Reuse simple WAV writer similar to AudioGenerationTask
    std::ofstream ofs(outPath, std::ios::binary);
    if (!ofs) return false;
    int32_t subchunk1Size = 16; int16_t audioFormat = 1; int16_t numChannels = channels;
    int32_t byteRate = sampleRate * numChannels * 2; int16_t blockAlign = numChannels * 2; int32_t bitsPerSample = 16;
    int32_t dataSize = static_cast<int32_t>(pcm.size()) * 2;
    ofs.write("RIFF",4); int32_t chunkSize = 36 + dataSize; ofs.write(reinterpret_cast<const char*>(&chunkSize),4); ofs.write("WAVE",4);
    ofs.write("fmt ",4); ofs.write(reinterpret_cast<const char*>(&subchunk1Size),4); ofs.write(reinterpret_cast<const char*>(&audioFormat),2);
    ofs.write(reinterpret_cast<const char*>(&numChannels),2); ofs.write(reinterpret_cast<const char*>(&sampleRate),4); ofs.write(reinterpret_cast<const char*>(&byteRate),4);
    ofs.write(reinterpret_cast<const char*>(&blockAlign),2); ofs.write(reinterpret_cast<const char*>(&bitsPerSample),2);
    ofs.write("data",4); ofs.write(reinterpret_cast<const char*>(&dataSize),4);
    for (size_t i = 0; i < pcm.size(); ++i) {
        float v = pcm[i]; if (v>1.0f) v=1.0f; if (v<-1.0f) v=-1.0f; int16_t s = static_cast<int16_t>(std::lround(v*32767.0f)); ofs.write(reinterpret_cast<const char*>(&s), sizeof(s));
    }
    return true;
}

void SpeechSynthesisTask::log(const std::string& message) const { std::cout << "[SpeechSynthesisTask] " << message << std::endl; }
