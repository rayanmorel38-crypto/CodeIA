#include "../../include/tasks/analysis_task.h"
#include <iostream>
#include "utils/logger.h"

AnalysisTask::AnalysisTask() : configured_(false) {}
AnalysisTask::~AnalysisTask() {}

void AnalysisTask::configure(const std::string& mode) {
    mode_ = mode;
    configured_ = true;
    log("Mode: " + mode_);
}

std::string AnalysisTask::run(const std::string& dataPath) {
    if (!configured_) throw std::runtime_error("AnalysisTask non configurée");
    log("Analyzing: " + dataPath);
    // Simulated analysis report
    return "analysis_report: OK";
}

void AnalysisTask::log(const std::string& message) const {
    cpp_engine::utils::Logger::instance().info("[AnalysisTask] " + message);
}
