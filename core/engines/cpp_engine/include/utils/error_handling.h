#ifndef CPP_ENGINE_UTILS_ERROR_HANDLING_H
#define CPP_ENGINE_UTILS_ERROR_HANDLING_H

#include <string>
#include <exception>

namespace cppengine {
namespace utils {

/**
 * Custom exceptions for C++ Engine
 */

class EngineException : public std::exception {
public:
    explicit EngineException(const std::string& message) : message_(message) {}
    const char* what() const noexcept override { return message_.c_str(); }
protected:
    std::string message_;
};

class GenerationException : public EngineException {
public:
    explicit GenerationException(const std::string& message) : EngineException(message) {}
};

class FilterException : public EngineException {
public:
    explicit FilterException(const std::string& message) : EngineException(message) {}
};

class EffectsException : public EngineException {
public:
    explicit EffectsException(const std::string& message) : EngineException(message) {}
};

class OptimizationException : public EngineException {
public:
    explicit OptimizationException(const std::string& message) : EngineException(message) {}
};

class ConfigException : public EngineException {
public:
    explicit ConfigException(const std::string& message) : EngineException(message) {}
};

} // namespace utils
} // namespace cppengine

#endif // CPP_ENGINE_UTILS_ERROR_HANDLING_H
