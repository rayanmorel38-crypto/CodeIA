#include "../../include/tasks/speech_recognition_task.h"
#include <iostream>
#include "utils/logger.h"

SpeechRecognitionTask::SpeechRecognitionTask() : configured_(false) {}
SpeechRecognitionTask::~SpeechRecognitionTask() {}

void SpeechRecognitionTask::configure(const std::string& model) { model_ = model; configured_ = true; log("Model: " + model_); }

std::string SpeechRecognitionTask::run(const std::string& audioPath) {
    if (!configured_) throw std::runtime_error("SpeechRecognitionTask non configurée");
    log("Recognizing: " + audioPath);
    // Simulated transcription
    return "transcribed text (simulated)";
}

void SpeechRecognitionTask::log(const std::string& message) const {
    cpp_engine::utils::Logger::instance().info("[SpeechRecognitionTask] " + message);
}
