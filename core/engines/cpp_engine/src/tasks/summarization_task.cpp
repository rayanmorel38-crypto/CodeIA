#include "../../include/tasks/summarization_task.h"
#include "utils/logger.h"

SummarizationTask::SummarizationTask() : configured_(false) {}
SummarizationTask::~SummarizationTask() {}

void SummarizationTask::configure(const std::string& method) {
    method_ = method;
    configured_ = true;
    log("Méthode de résumé configurée: " + method_);
}

std::string SummarizationTask::run(const std::string& input) {
    if (!configured_) throw std::runtime_error("Tâche non configurée");
    log("Résumé de: " + input);
    return "Résumé simulé";
}

void SummarizationTask::log(const std::string& message) const {
    cpp_engine::utils::Logger::instance().info(std::string("[SummarizationTask] ") + message);
}
