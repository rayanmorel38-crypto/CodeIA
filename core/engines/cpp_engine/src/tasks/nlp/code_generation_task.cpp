#include "../../include/tasks/code_generation_task.h"
#include <iostream>

CodeGenerationTask::CodeGenerationTask() : configured_(false) {}
CodeGenerationTask::~CodeGenerationTask() {}

void CodeGenerationTask::configure(const std::string& language) {
    language_ = language;
    configured_ = true;
    log("Langage configuré: " + language_);
}

std::string CodeGenerationTask::run(const std::string& prompt) {
    if (!configured_) throw std::runtime_error("Tâche non configurée");
    log("Génération de code pour: " + prompt);
    return "Code simulé";
}

void CodeGenerationTask::log(const std::string& message) const {
    std::cout << "[CodeGenerationTask] " << message << std::endl;
}
