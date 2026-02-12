#include "../../include/tasks/physics_simulation_task.h"
#include <iostream>

PhysicsSimulationTask::PhysicsSimulationTask() : configured_(false) {}
PhysicsSimulationTask::~PhysicsSimulationTask() {}

void PhysicsSimulationTask::configure(const std::string& engineName) {
    engineName_ = engineName;
    configured_ = true;
    log("Physics engine: " + engineName_);
}

bool PhysicsSimulationTask::run(double timestep) {
    if (!configured_) throw std::runtime_error("PhysicsSimulationTask non configurée");
    log("Simulation step, dt=" + std::to_string(timestep));
    return true;
}

void PhysicsSimulationTask::log(const std::string& message) const {
    std::cout << "[PhysicsSimulationTask] " << message << std::endl;
}
