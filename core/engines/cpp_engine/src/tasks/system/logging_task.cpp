#include "../../include/tasks/logging_task.h"
#include "utils/logger.h"
#include <fstream>

LoggingTask::LoggingTask() : configured_(false) {}
LoggingTask::~LoggingTask() {}

void LoggingTask::configure(const std::string& logFile) {
    logFile_ = logFile;
    configured_ = true;
    log("Fichier de log: " + logFile_);
}

bool LoggingTask::run(const std::string& message) {
    if (!configured_) throw std::runtime_error("Tâche non configurée");
    std::ofstream ofs(logFile_, std::ios::app);
    if (!ofs) {
        log("Erreur ouverture fichier log");
        return false;
    }
    ofs << message << std::endl;
    log("Message loggé: " + message);
    return true;
}

void LoggingTask::log(const std::string& message) const {
    cpp_engine::utils::Logger::instance().info(std::string("[LoggingTask] ") + message);
}
