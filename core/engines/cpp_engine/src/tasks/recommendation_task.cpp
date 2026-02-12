#include "../../include/tasks/recommendation_task.h"
#include <iostream>

RecommendationTask::RecommendationTask() : configured_(false) {}
RecommendationTask::~RecommendationTask() {}

void RecommendationTask::configure(const std::vector<std::string>& items) {
    items_ = items;
    configured_ = true;
    log("Items configurés: " + std::to_string(items_.size()));
}

std::vector<std::string> RecommendationTask::run(const std::string& userId) {
    if (!configured_) throw std::runtime_error("Tâche non configurée");
    log("Recommandation pour utilisateur: " + userId);
    std::vector<std::string> result;
    if (!items_.empty()) result.push_back(items_[0]);
    return result;
}

void RecommendationTask::log(const std::string& message) const {
    std::cout << "[RecommendationTask] " << message << std::endl;
}
