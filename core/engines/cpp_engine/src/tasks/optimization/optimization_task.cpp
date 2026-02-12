#include "../../include/tasks/optimization_task.h"
#include "../../include/tasks/optimization_task.h"
#include <iostream>

OptimizationTask::OptimizationTask() : configured_(false) {}
OptimizationTask::~OptimizationTask() {}

void OptimizationTask::configure(const std::string& method) { method_ = method; configured_ = true; log("Method: " + method_); }

bool OptimizationTask::run(const std::string& problemSpec) {
    if (!configured_) throw std::runtime_error("OptimizationTask non configurée");
    log("Running optimization: " + problemSpec);
    // Simulated success
    return true;
}

void OptimizationTask::log(const std::string& message) const { std::cout << "[OptimizationTask] " << message << std::endl; }
