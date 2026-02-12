#include "utils/logger.h"
#include <iostream>
#include <chrono>
#include <iomanip>

namespace cpp_engine {
namespace utils {

Logger& Logger::instance() {
    static Logger instance;
    return instance;
}

void Logger::init(const std::string& log_file, LogLevel level) {
    level_ = level;
    log_file_.open(log_file, std::ios::app);
}

void Logger::log(LogLevel level, const std::string& message) {
    if (level < level_) return;
    
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    
    std::string level_str;
    switch (level) {
        case LogLevel::DEBUG: level_str = "DEBUG"; break;
        case LogLevel::INFO: level_str = "INFO"; break;
        case LogLevel::WARNING: level_str = "WARNING"; break;
        case LogLevel::ERROR: level_str = "ERROR"; break;
        case LogLevel::CRITICAL: level_str = "CRITICAL"; break;
    }
    
    std::string log_message = "[" + ss.str() + "] [" + level_str + "] " + message;
    // Write only to log file to avoid direct console prints (centralized logging)
    if (log_file_.is_open()) {
        log_file_ << log_message << std::endl;
        log_file_.flush();
    }
}

void Logger::debug(const std::string& message) {
    log(LogLevel::DEBUG, message);
}

void Logger::info(const std::string& message) {
    log(LogLevel::INFO, message);
}

void Logger::warning(const std::string& message) {
    log(LogLevel::WARNING, message);
}

void Logger::error(const std::string& message) {
    log(LogLevel::ERROR, message);
}

void Logger::critical(const std::string& message) {
    log(LogLevel::CRITICAL, message);
}

void Logger::set_level(LogLevel level) {
    level_ = level;
}

LogLevel Logger::get_level() const {
    return level_;
}

} // namespace utils
} // namespace cpp_engine
