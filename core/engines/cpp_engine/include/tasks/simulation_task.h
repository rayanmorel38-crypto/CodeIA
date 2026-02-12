#pragma once
/**
 * @class SimulationTask
 * @brief Simulation de processus ou d'environnements.
 */
#include <string>
#include <stdexcept>

class SimulationTask {
public:
    SimulationTask();
    ~SimulationTask();
    void configure(const std::string& scenario);
    bool run();
    void log(const std::string& message) const;
private:
    std::string scenario_;
    bool configured_;
};
