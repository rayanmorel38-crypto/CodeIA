#include "../../include/tasks/calibration_task.h"
#include "utils/logger.h"

CalibrationTask::CalibrationTask() : configured_(false) {}
CalibrationTask::~CalibrationTask() {}

void CalibrationTask::configure(const std::string& target) {
    target_ = target;
    configured_ = true;
    log("Cible de calibration: " + target_);
}

bool CalibrationTask::run() {
    if (!configured_) throw std::runtime_error("Tâche non configurée");
    log("Calibration en cours...");
    return true;
}

void CalibrationTask::log(const std::string& message) const {
    cpp_engine::utils::Logger::instance().info(std::string("[CalibrationTask] ") + message);
}
