#ifndef CPP_ENGINE_UTILS_LOGGER_H
#define CPP_ENGINE_UTILS_LOGGER_H

#include <string>
#include <fstream>
#include <memory>

namespace cpp_engine {
namespace utils {

/**
 * Logger - Structured logging for C++ Engine
 */
enum class LogLevel {
    DEBUG = 0,
    INFO = 1,
    WARNING = 2,
    ERROR = 3,
    CRITICAL = 4
};

class Logger {
public:
    static Logger& instance();
    
    void init(const std::string& log_file, LogLevel level = LogLevel::INFO);
    
    void debug(const std::string& message);
    void info(const std::string& message);
    void warning(const std::string& message);
    void error(const std::string& message);
    void critical(const std::string& message);
    
    void set_level(LogLevel level);
    LogLevel get_level() const;
    
private:
    Logger() = default;
    void log(LogLevel level, const std::string& message);
    
    std::ofstream log_file_;
    LogLevel level_;
};

} // namespace utils
} // namespace cpp_engine

#endif // CPP_ENGINE_UTILS_LOGGER_H
