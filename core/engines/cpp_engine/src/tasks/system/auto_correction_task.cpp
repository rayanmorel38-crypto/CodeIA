#include "../../include/tasks/auto_correction_task.h"
#include "utils/logger.h"

AutoCorrectionTask::AutoCorrectionTask() : configured_(false) {}
AutoCorrectionTask::~AutoCorrectionTask() {}

void AutoCorrectionTask::configure(const std::string& mode) {
    mode_ = mode;
    configured_ = true;
    log("Mode de correction: " + mode_);
}

std::string AutoCorrectionTask::run(const std::string& input) {
    if (!configured_) throw std::runtime_error("Tâche non configurée");
    log("Correction de: " + input);
    return "Correction simulée";
}

void AutoCorrectionTask::log(const std::string& message) const {
    cpp_engine::utils::Logger::instance().info(std::string("[AutoCorrectionTask] ") + message);
}
