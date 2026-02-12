#include "../../include/tasks/graph_generation_task.h"
#include <iostream>

GraphGenerationTask::GraphGenerationTask() : configured_(false) {}
GraphGenerationTask::~GraphGenerationTask() {}

void GraphGenerationTask::configure(const std::string& type) {
    type_ = type;
    configured_ = true;
    log("Type de graphe: " + type_);
}

bool GraphGenerationTask::run(const std::string& outputPath) {
    if (!configured_) throw std::runtime_error("Tâche non configurée");
    log("Génération de graphe vers " + outputPath);
    return true;
}

void GraphGenerationTask::log(const std::string& message) const {
    std::cout << "[GraphGenerationTask] " << message << std::endl;
}
