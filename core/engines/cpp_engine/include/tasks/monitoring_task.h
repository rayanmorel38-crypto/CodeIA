#pragma once
/**
 * @class MonitoringTask
 * @brief Tâche de monitoring et supervision.
 */
#include <string>
#include <stdexcept>

class MonitoringTask {
public:
    MonitoringTask();
    ~MonitoringTask();
    void configure(const std::string& target);
    bool run();
    void log(const std::string& message) const;
private:
    std::string target_;
    bool configured_;
};
