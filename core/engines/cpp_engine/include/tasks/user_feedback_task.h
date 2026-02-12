#pragma once
/**
 * @class UserFeedbackTask
 * @brief Tâche de gestion du feedback utilisateur.
 */
#include <string>
#include <stdexcept>

class UserFeedbackTask {
public:
    UserFeedbackTask();
    ~UserFeedbackTask();
    void configure(const std::string& mode);
    bool run(const std::string& feedback);
    void log(const std::string& message) const;
private:
    std::string mode_;
    bool configured_;
};
