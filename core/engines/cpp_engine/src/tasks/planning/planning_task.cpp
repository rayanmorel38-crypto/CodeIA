#include "../../include/tasks/planning_task.h"
#include <iostream>

PlanningTask::PlanningTask() : configured_(false) {}
PlanningTask::~PlanningTask() {}

void PlanningTask::configure(const std::string& strategy) { strategy_ = strategy; configured_ = true; log("Strategy: " + strategy_); }

std::string PlanningTask::run(const std::string& goal) {
    if (!configured_) throw std::runtime_error("PlanningTask non configurée");
    log("Planning for goal: " + goal);
    return "plan: [step1, step2]";
}

void PlanningTask::log(const std::string& message) const { std::cout << "[PlanningTask] " << message << std::endl; }
