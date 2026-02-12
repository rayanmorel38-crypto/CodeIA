#include "../../include/tasks/text_generation_task.h"
#include "utils/logger.h"

TextGenerationTask::TextGenerationTask() : configured_(false) {}
TextGenerationTask::~TextGenerationTask() {}

void TextGenerationTask::configure(const std::string& model) {
    model_ = model;
    configured_ = true;
    log("Model set: " + model_);
}

std::string TextGenerationTask::run(const std::string& prompt) {
    if (!configured_) throw std::runtime_error("TextGenerationTask non configurée");
    log("Generating text for: " + prompt);
    // Simulated response
    std::string resp = "[[generated text based on prompt: " + prompt + "]]";
    return resp;
}

void TextGenerationTask::log(const std::string& message) const {
    cpp_engine::utils::Logger::instance().info(std::string("[TextGenerationTask] ") + message);
}
