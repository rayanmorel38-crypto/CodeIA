#pragma once
/**
 * @class LoggingTask
 * @brief Tâche de gestion des logs.
 */
#include <string>
#include <stdexcept>

class LoggingTask {
public:
    LoggingTask();
    ~LoggingTask();
    void configure(const std::string& logFile);
    bool run(const std::string& message);
    void log(const std::string& message) const;
private:
    std::string logFile_;
    bool configured_;
};
