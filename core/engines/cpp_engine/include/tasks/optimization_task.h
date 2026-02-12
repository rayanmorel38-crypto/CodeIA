#pragma once
#include <string>
#include <stdexcept>

/**
 * @class OptimizationTask
 * @brief Tâche d'optimisation (ex.: hyperparam tuning, optimisation locale).
 */
class OptimizationTask {
public:
    OptimizationTask();
    ~OptimizationTask();
    void configure(const std::string& method = "default");
    bool run(const std::string& problemSpec);
    void log(const std::string& message) const;
private:
    std::string method_;
    bool configured_;
};
