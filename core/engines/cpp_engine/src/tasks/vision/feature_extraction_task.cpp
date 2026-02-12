#include "../../include/tasks/feature_extraction_task.h"
#include "utils/logger.h"

FeatureExtractionTask::FeatureExtractionTask() : configured_(false) {}
FeatureExtractionTask::~FeatureExtractionTask() {}

void FeatureExtractionTask::configure(const std::vector<std::string>& features) {
    features_ = features;
    configured_ = true;
    log("Configuration des features: " + std::to_string(features_.size()));
}

std::vector<float> FeatureExtractionTask::run(const std::string& dataPath) {
    if (!configured_) throw std::runtime_error("Tâche non configurée");
    log("Extraction sur " + dataPath);
    std::vector<float> result(features_.size(), 1.0f); // Simulé
    return result;
}

void FeatureExtractionTask::log(const std::string& message) const {
    cpp_engine::utils::Logger::instance().info(std::string("[FeatureExtractionTask] ") + message);
}
