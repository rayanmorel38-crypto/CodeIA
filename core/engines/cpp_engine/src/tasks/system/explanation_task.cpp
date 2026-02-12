#include "../../include/tasks/explanation_task.h"
#include "utils/logger.h"

ExplanationTask::ExplanationTask() : configured_(false) {}
ExplanationTask::~ExplanationTask() {}

void ExplanationTask::configure(const std::string& method) {
    method_ = method;
    configured_ = true;
    log("Méthode d'explication: " + method_);
}

std::string ExplanationTask::run(const std::string& input) {
    if (!configured_) throw std::runtime_error("Tâche non configurée");
    log("Explication pour: " + input);
    return "Explication simulée";
}

void ExplanationTask::log(const std::string& message) const {
    cpp_engine::utils::Logger::instance().info(std::string("[ExplanationTask] ") + message);
}
