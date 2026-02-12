#include "config.h"
#include <fstream>
#include <iostream>
#include <filesystem>
#include "utils.h"

namespace config {

Config::Config() {}
Config::~Config() {}

bool Config::loadFromFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) return false;
    data_.clear();

    std::string line;
    while (std::getline(file, line)) {
        // Ignore comments and blank lines
        std::string trimmed = utils::trim(line);
        if (trimmed.empty() || trimmed[0] == '#') continue;
        auto pos = trimmed.find('=');
        if (pos != std::string::npos) {
            std::string key = utils::trim(trimmed.substr(0, pos));
            std::string val = utils::trim(trimmed.substr(pos + 1));
            data_[key] = val;
        }
    }
    file.close();
    return true;
}

bool Config::saveToFile(const std::string& path) const {
    // Write atomically to temporary file then rename
    try {
        std::filesystem::path p(path);
        auto tmp = p;
        tmp += ".tmp";
        std::ofstream file(tmp.string());
        if (!file.is_open()) return false;
        for (auto& kv : data_) {
            file << kv.first << "=" << kv.second << "\n";
        }
        file.close();
        std::filesystem::rename(tmp, p);
        return true;
    } catch (...) {
        return false;
    }
}

std::string Config::getValue(const std::string& key) const {
    auto it = data_.find(key);
    return (it != data_.end()) ? it->second : "";
}

void Config::setValue(const std::string& key, const std::string& value) {
    data_[key] = value;
}

} // namespace config

