#include "../../include/tasks/scoring_task.h"
#include "utils/logger.h"

ScoringTask::ScoringTask() : configured_(false) {}
ScoringTask::~ScoringTask() {}

void ScoringTask::configure(const std::string& metric) {
    metric_ = metric;
    configured_ = true;
    log("Métrique: " + metric_);
}

std::vector<float> ScoringTask::run(const std::vector<std::string>& items) {
    if (!configured_) throw std::runtime_error("Tâche non configurée");
    log("Scoring de " + std::to_string(items.size()) + " items");
    std::vector<float> result(items.size(), 1.0f); // Simulé
    return result;
}

void ScoringTask::log(const std::string& message) const {
    cpp_engine::utils::Logger::instance().info(std::string("[ScoringTask] ") + message);
}
