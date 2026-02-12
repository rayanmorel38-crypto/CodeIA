#include "../../include/tasks/anonymization_task.h"
#include "utils/logger.h"

AnonymizationTask::AnonymizationTask() : configured_(false) {}
AnonymizationTask::~AnonymizationTask() {}

void AnonymizationTask::configure(const std::string& method) {
    method_ = method;
    configured_ = true;
    log("Méthode d'anonymisation: " + method_);
}

std::string AnonymizationTask::run(const std::string& input) {
    if (!configured_) throw std::runtime_error("Tâche non configurée");
    log("Anonymisation de: " + input);
    return "Donnée anonymisée";
}

void AnonymizationTask::log(const std::string& message) const {
    cpp_engine::utils::Logger::instance().info(std::string("[AnonymizationTask] ") + message);
}
