#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <filesystem>
#include "utils/logger.h"

namespace fs = std::filesystem;

namespace cpp_engine { namespace modules { namespace system {

class Configuration {
public:
    static std::string name() { return "configuration"; }

    static std::unordered_map<std::string,std::string> load_kv(const std::string &path) {
        std::unordered_map<std::string,std::string> kv;
        try {
            std::ifstream in(path);
            if (!in) { cpp_engine::utils::Logger::instance().warning(std::string("[Configuration] no file: ") + path); return kv; }
            std::string line;
            while (std::getline(in, line)) {
                auto pos = line.find('=');
                if (pos==std::string::npos) continue;
                std::string k = line.substr(0,pos);
                std::string v = line.substr(pos+1);
                kv[k]=v;
            }
        } catch(...) {}
        cpp_engine::utils::Logger::instance().info(std::string("[Configuration] loaded ") + std::to_string(kv.size()) + std::string(" entries from ") + path);
        return kv;
    }
};

} } }

extern "C" void register_module() {
    cpp_engine::modules::system::Configuration::load_kv("config/example.cfg");
}
