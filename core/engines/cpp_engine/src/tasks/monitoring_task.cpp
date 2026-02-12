#include "../../include/tasks/monitoring_task.h"
#include "utils/logger.h"

MonitoringTask::MonitoringTask() : configured_(false) {}
MonitoringTask::~MonitoringTask() {}

void MonitoringTask::configure(const std::string& target) {
    target_ = target;
    configured_ = true;
    log("Cible de monitoring: " + target_);
}

bool MonitoringTask::run() {
    if (!configured_) throw std::runtime_error("Tâche non configurée");
    log("Monitoring en cours...");
    return true;
}

void MonitoringTask::log(const std::string& message) const {
    cpp_engine::utils::Logger::instance().info(std::string("[MonitoringTask] ") + message);
}
