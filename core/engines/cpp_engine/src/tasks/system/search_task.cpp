#include "../../include/tasks/search_task.h"
#include "utils/logger.h"

SearchTask::SearchTask() : configured_(false) {}
SearchTask::~SearchTask() {}

void SearchTask::configure(const std::string& queryType) {
    queryType_ = queryType;
    configured_ = true;
    log("Type de recherche configuré: " + queryType_);
}

std::vector<std::string> SearchTask::run(const std::string& query) {
    if (!configured_) throw std::runtime_error("Tâche non configurée");
    log("Recherche: " + query);
    std::vector<std::string> result;
    result.push_back("Résultat simulé");
    return result;
}

void SearchTask::log(const std::string& message) const {
    cpp_engine::utils::Logger::instance().info(std::string("[SearchTask] ") + message);
}
