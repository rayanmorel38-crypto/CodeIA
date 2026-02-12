#pragma once
/**
 * @class PhysicsSimulationTask
 * @brief Tâche de simulation physique (moteur physique).
 */
#include <string>
#include <stdexcept>

class PhysicsSimulationTask {
public:
    PhysicsSimulationTask();
    ~PhysicsSimulationTask();
    void configure(const std::string& engineName);
    bool run(double timestep);
    void log(const std::string& message) const;
private:
    std::string engineName_;
    bool configured_;
};
