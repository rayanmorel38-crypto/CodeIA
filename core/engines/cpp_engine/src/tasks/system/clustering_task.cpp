#include "../../include/tasks/clustering_task.h"
#include "utils/logger.h"

ClusteringTask::ClusteringTask() : nClusters_(2), configured_(false) {}
ClusteringTask::~ClusteringTask() {}

void ClusteringTask::configure(int nClusters) {
    nClusters_ = nClusters;
    configured_ = true;
    log("Nombre de clusters: " + std::to_string(nClusters_));
}

std::vector<int> ClusteringTask::run(const std::vector<std::vector<float>>& data) {
    if (!configured_) throw std::runtime_error("Tâche non configurée");
    log("Clustering sur " + std::to_string(data.size()) + " éléments");
    std::vector<int> result(data.size(), 0); // Simulé
    return result;
}

void ClusteringTask::log(const std::string& message) const {
    cpp_engine::utils::Logger::instance().info(std::string("[ClusteringTask] ") + message);
}
