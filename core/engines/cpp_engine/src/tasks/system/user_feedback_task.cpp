#include "../../include/tasks/user_feedback_task.h"
#include <iostream>
#include "utils/logger.h"

UserFeedbackTask::UserFeedbackTask() : configured_(false) {}
UserFeedbackTask::~UserFeedbackTask() {}

void UserFeedbackTask::configure(const std::string& mode) {
    mode_ = mode;
    configured_ = true;
    log("Mode de feedback: " + mode_);
}

bool UserFeedbackTask::run(const std::string& feedback) {
    if (!configured_) throw std::runtime_error("Tâche non configurée");
    log("Feedback reçu: " + feedback);
    return true;
}

void UserFeedbackTask::log(const std::string& message) const {
    cpp_engine::utils::Logger::instance().info("[UserFeedbackTask] " + message);
}
