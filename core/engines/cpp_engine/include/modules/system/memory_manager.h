#ifndef CPP_ENGINE_MODULES_SYSTEM_MEMORY_MANAGER_H
#define CPP_ENGINE_MODULES_SYSTEM_MEMORY_MANAGER_H

#include <string>
#include <vector>
#include <map>

namespace cppengine { namespace modules { namespace system {

struct MemoryEntry {
    std::string timestamp;
    std::string type;
    std::string key;
    std::string value;
};

class MemoryManager {
public:
    static MemoryManager& instance();
    void init(const std::string& db_path = "memory.db");
    void store(const std::string& type, const std::string& key, const std::string& value);
    std::string recall(const std::string& key);
    std::vector<MemoryEntry> search(const std::string& type = "");
    void learn_pattern(const std::string& input, const std::string& response);
    std::string get_learned_response(const std::string& input);

private:
    MemoryManager();
    ~MemoryManager();
    
    std::map<std::string, MemoryEntry> memories_;
    std::string db_path_;
    
    void load_memory();
    void save_memory();
};

} } }

#endif // CPP_ENGINE_MODULES_SYSTEM_MEMORY_MANAGER_H