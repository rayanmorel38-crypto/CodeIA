#include "../../include/tasks/classification_task.h"
#include "utils/logger.h"

ClassificationTask::ClassificationTask() : configured_(false) {}
ClassificationTask::~ClassificationTask() {}

void ClassificationTask::configure(const std::string& model) {
    model_ = model; configured_ = true; log("Model: " + model_);
}

std::vector<int> ClassificationTask::run(const std::string& inputPath) {
    if (!configured_) throw std::runtime_error("ClassificationTask non configurée");
    log("Classifying: " + inputPath);
    // Simulated: return single label 0
    return std::vector<int>{0};
}

void ClassificationTask::log(const std::string& message) const {
    cpp_engine::utils::Logger::instance().info(std::string("[ClassificationTask] ") + message);
}
