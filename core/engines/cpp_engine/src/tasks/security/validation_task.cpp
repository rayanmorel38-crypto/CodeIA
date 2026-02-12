#include "../../include/tasks/validation_task.h"
#include <iostream>
#include "utils/logger.h"

ValidationTask::ValidationTask() : configured_(false) {}
ValidationTask::~ValidationTask() {}

void ValidationTask::configure(const std::string& type) {
    type_ = type;
    configured_ = true;
    log("Type de validation: " + type_);
}

bool ValidationTask::run(const std::string& input) {
    if (!configured_) throw std::runtime_error("Tâche non configurée");
    log("Validation de: " + input);
    return true;
}

void ValidationTask::log(const std::string& message) const {
    cpp_engine::utils::Logger::instance().info("[ValidationTask] " + message);
}
