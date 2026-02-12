#include "../../include/tasks/translation_task.h"
#include <iostream>
#include "utils/logger.h"

TranslationTask::TranslationTask() : configured_(false) {}
TranslationTask::~TranslationTask() {}

void TranslationTask::configure(const std::string& targetLang) { targetLang_ = targetLang; configured_ = true; log("Target: " + targetLang_); }

std::string TranslationTask::run(const std::string& text) {
    if (!configured_) throw std::runtime_error("TranslationTask non configurée");
    log("Translating text to " + targetLang_);
    return text + " [translated->" + targetLang_ + "]";
}

void TranslationTask::log(const std::string& message) const {
    cpp_engine::utils::Logger::instance().info("[TranslationTask] " + message);
}
