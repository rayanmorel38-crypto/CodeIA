#pragma once

#include <string>

#define CPP_ENGINE_VERSION "1.0.0"
#define CPP_ENGINE_NAME "CppEngine"
#define CPP_ENGINE_MAX_THREADS 8

#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define ARRAY_SIZE(a) (sizeof(a)/sizeof((a)[0]))

// Logging macros
namespace macros {
    void printInfo(const std::string& message);
    void printWarning(const std::string& message);
    void printError(const std::string& message);
}

#define LOG_INFO(msg) macros::printInfo(msg)
#define LOG_WARN(msg) macros::printWarning(msg)
#define LOG_ERROR(msg) macros::printError(msg)
