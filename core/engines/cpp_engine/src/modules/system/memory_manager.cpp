#include "modules/system/memory_manager.h"
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <chrono>
#include "utils/logger.h"

namespace cppengine { namespace modules { namespace system {

MemoryManager::MemoryManager() {}

MemoryManager::~MemoryManager() {}

MemoryManager& MemoryManager::instance() {
    static MemoryManager instance;
    return instance;
}

void MemoryManager::init(const std::string& db_path) {
    db_path_ = db_path;
    load_memory();
    cpp_engine::utils::Logger::instance().info("[MemoryManager] initialized with " + std::to_string(memories_.size()) + " entries");
}

void MemoryManager::store(const std::string& type, const std::string& key, const std::string& value) {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::string timestamp = std::ctime(&time_t);
    timestamp.pop_back(); // remove newline

    MemoryEntry entry{timestamp, type, key, value};
    memories_[key] = entry;
    save_memory();
    cpp_engine::utils::Logger::instance().info("[MemoryManager] stored: " + type + " " + key);
}

std::string MemoryManager::recall(const std::string& key) {
    if (memories_.count(key)) {
        return memories_[key].value;
    }
    return "";
}

std::vector<MemoryEntry> MemoryManager::search(const std::string& type) {
    std::vector<MemoryEntry> results;
    for (const auto& pair : memories_) {
        if (type.empty() || pair.second.type == type) {
            results.push_back(pair.second);
        }
    }
    return results;
}

void MemoryManager::learn_pattern(const std::string& input, const std::string& response) {
    store("learning", "pattern_" + input, response);
}

std::string MemoryManager::get_learned_response(const std::string& input) {
    return recall("pattern_" + input);
}

void MemoryManager::load_memory() {
    std::ifstream file(db_path_);
    if (!file.is_open()) return;

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string timestamp, type, key, value;
        if (std::getline(iss, timestamp, '|') &&
            std::getline(iss, type, '|') &&
            std::getline(iss, key, '|') &&
            std::getline(iss, value)) {
            memories_[key] = {timestamp, type, key, value};
        }
    }
}

void MemoryManager::save_memory() {
    std::ofstream file(db_path_);
    for (const auto& pair : memories_) {
        const auto& entry = pair.second;
        file << entry.timestamp << "|" << entry.type << "|" << entry.key << "|" << entry.value << "\n";
    }
}

} } }