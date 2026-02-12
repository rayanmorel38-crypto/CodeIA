#ifndef MACROS_H
#define MACROS_H

#include <string>

namespace macros {

#define LOG_INFO(msg) printInfo(msg)
#define LOG_WARNING(msg) printWarning(msg)
#define LOG_ERROR(msg) printError(msg)

void printInfo(const std::string& message);
void printWarning(const std::string& message);
void printError(const std::string& message);

} // namespace macros

#endif

