#include "../../include/tasks/security_task.h"
#include <iostream>
#include "utils/logger.h"

SecurityTask::SecurityTask() : configured_(false) {}
SecurityTask::~SecurityTask() {}

void SecurityTask::configure(const std::string& policy) {
    policy_ = policy;
    configured_ = true;
    log("Politique de sécurité: " + policy_);
}

bool SecurityTask::run(const std::string& resource) {
    if (!configured_) throw std::runtime_error("Tâche non configurée");
    log("Vérification sécurité sur: " + resource);
    return true;
}

void SecurityTask::log(const std::string& message) const {
    cpp_engine::utils::Logger::instance().info("[SecurityTask] " + message);
}
