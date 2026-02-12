#include "utils.h"
#include <algorithm>
#include <cctype>
#include <locale>

namespace utils {

std::string toUpper(const std::string& input) {
    std::string res = input;
    std::transform(res.begin(), res.end(), res.begin(), ::toupper);
    return res;
}

std::string toLower(const std::string& input) {
    std::string res = input;
    std::transform(res.begin(), res.end(), res.begin(), ::tolower);
    return res;
}

bool isNumeric(const std::string& input) {
    return !input.empty() && std::all_of(input.begin(), input.end(), ::isdigit);
}

// Trim helpers
static inline void ltrim_inplace(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}

static inline void rtrim_inplace(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

std::string ltrim(const std::string& s) {
    std::string tmp = s;
    ltrim_inplace(tmp);
    return tmp;
}

std::string rtrim(const std::string& s) {
    std::string tmp = s;
    rtrim_inplace(tmp);
    return tmp;
}

std::string trim(const std::string& s) {
    std::string tmp = s;
    ltrim_inplace(tmp);
    rtrim_inplace(tmp);
    return tmp;
}

} // namespace utils

