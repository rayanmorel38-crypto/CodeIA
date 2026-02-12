#ifndef UTILS_H
#define UTILS_H

#include <string>

namespace utils {

std::string toUpper(const std::string& input);
std::string toLower(const std::string& input);
bool isNumeric(const std::string& input);
// Trim helpers
std::string ltrim(const std::string& s);
std::string rtrim(const std::string& s);
std::string trim(const std::string& s);

} // namespace utils

#endif

