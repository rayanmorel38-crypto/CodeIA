#include "../../include/tasks/graph_optimization_task.h"
#include <iostream>

GraphOptimizationTask::GraphOptimizationTask() : configured_(false) {}
GraphOptimizationTask::~GraphOptimizationTask() {}

void GraphOptimizationTask::configure(const std::string& algorithm) {
    algorithm_ = algorithm;
    configured_ = true;
    log("Graph optimization alg: " + algorithm_);
}

std::vector<int> GraphOptimizationTask::run(const std::vector<std::pair<int,int>>& edges) {
    if (!configured_) throw std::runtime_error("GraphOptimizationTask non configurée");
    log("Optimisation graphe: edges=" + std::to_string(edges.size()));
    std::vector<int> result; result.reserve(edges.size());
    for (size_t i = 0; i < edges.size(); ++i) result.push_back((int)i);
    return result;
}

void GraphOptimizationTask::log(const std::string& message) const {
    std::cout << "[GraphOptimizationTask] " << message << std::endl;
}
