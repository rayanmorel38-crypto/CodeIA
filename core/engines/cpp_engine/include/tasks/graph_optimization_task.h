#pragma once
/**
 * @class GraphOptimizationTask
 * @brief Algorithmes lourds d'optimisation de graphes (C++ haute-perf).
 */
#include <string>
#include <stdexcept>
#include <vector>

class GraphOptimizationTask {
public:
    GraphOptimizationTask();
    ~GraphOptimizationTask();
    void configure(const std::string& algorithm);
    std::vector<int> run(const std::vector<std::pair<int,int>>& edges);
    void log(const std::string& message) const;
private:
    std::string algorithm_;
    bool configured_;
};
