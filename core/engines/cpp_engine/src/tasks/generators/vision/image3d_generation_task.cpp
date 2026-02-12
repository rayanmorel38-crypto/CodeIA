#include "../../include/tasks/image3d_generation_task.h"
#include <iostream>
#include "utils/logger.h"

Image3DGenerationTask::Image3DGenerationTask() : configured_(false) {}
Image3DGenerationTask::~Image3DGenerationTask() {}

void Image3DGenerationTask::configure(const std::string& modelPath) {
    modelPath_ = modelPath;
    configured_ = true;
    log("Modèle 3D configuré: " + modelPath_);
}

bool Image3DGenerationTask::run(const std::string& outputPath) {
    if (!configured_) throw std::runtime_error("Tâche non configurée");
    log("Génération 3D vers " + outputPath);
    return true;
}

void Image3DGenerationTask::log(const std::string& message) const {
    cpp_engine::utils::Logger::instance().info("[Image3DGenerationTask] " + message);
}
