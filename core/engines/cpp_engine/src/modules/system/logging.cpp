#include <iostream>
#include <fstream>
#include <string>
#include <mutex>
#include <filesystem>
#include "utils/logger.h"

namespace fs = std::filesystem;

namespace cpp_engine { namespace modules { namespace system {

class SysLogging {
public:
    static SysLogging& instance() {
        static SysLogging inst;
        return inst;
    }

    static std::string name() { return "system_logging"; }

    void log(const std::string &msg) {
        std::lock_guard<std::mutex> lk(mtx_);
        if (!ofs_.is_open()) open_log();
        ofs_ << msg << std::endl;
        cpp_engine::utils::Logger::instance().info(std::string("[SysLogging] ") + msg);
    }

private:
    SysLogging() { open_log(); }
    ~SysLogging(){ if (ofs_.is_open()) ofs_.close(); }

    void open_log() {
        try {
            fs::create_directories("logs");
            ofs_.open("logs/system.log", std::ios::app);
        } catch(...) {}
    }

    std::ofstream ofs_;
    std::mutex mtx_;
};

} } }

extern "C" void register_module() {
    cpp_engine::modules::system::SysLogging::instance().log("init");
}
