#include "../../include/tasks/simulation_task.h"
#include <iostream>

SimulationTask::SimulationTask() : configured_(false) {}
SimulationTask::~SimulationTask() {}

void SimulationTask::configure(const std::string& scenario) {
    scenario_ = scenario;
    configured_ = true;
    log("Scénario configuré: " + scenario_);
}

bool SimulationTask::run() {
    if (!configured_) throw std::runtime_error("Tâche non configurée");
    log("Simulation en cours...");
    return true;
}

void SimulationTask::log(const std::string& message) const {
    std::cout << "[SimulationTask] " << message << std::endl;
}
