#include "../../include/tasks/object_detection_task.h"
#include "utils/logger.h"
#include <fstream>

ObjectDetectionTask::ObjectDetectionTask() : threshold_(0.5f), configured_(false) {}
ObjectDetectionTask::~ObjectDetectionTask() {}

void ObjectDetectionTask::configure(const std::string& modelPath, float threshold) {
    modelPath_ = modelPath;
    threshold_ = threshold;
    configured_ = true;
    log("Configuration du modèle: " + modelPath_ + ", seuil: " + std::to_string(threshold_));
}

std::vector<DetectionResult> ObjectDetectionTask::run(const std::string& imagePath) {
    if (!configured_) throw std::runtime_error("Tâche non configurée");
    log("Début de la détection sur " + imagePath);
    std::vector<DetectionResult> results;
    // Simulation d'une détection (à remplacer par l'appel au modèle réel)
    if (imagePath.find("person") != std::string::npos) {
        results.push_back({"person", 0.98f, 100, 150, 50, 120});
    }
    if (results.empty()) {
        log("Aucun objet détecté.");
    } else {
        log("Objets détectés: " + std::to_string(results.size()));
    }
    return results;
}

void ObjectDetectionTask::log(const std::string& message) const {
    cpp_engine::utils::Logger::instance().info(std::string("[ObjectDetectionTask] ") + message);
}
