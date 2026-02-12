#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include "utils/logger.h"

namespace fs = std::filesystem;

namespace cpp_engine { namespace modules { namespace system {

class UserFeedback {
public:
    static std::string name() { return "user_feedback"; }
    static void send(const std::string &feedback) {
        try {
            fs::create_directories("logs");
            std::ofstream out("logs/user_feedback.log", std::ios::app);
            out << feedback << std::endl;
        } catch(...) {}
        cpp_engine::utils::Logger::instance().info(std::string("[UserFeedback] ") + feedback);
    }
};

} } }

extern "C" void register_module() {
    cpp_engine::modules::system::UserFeedback::send("ok");
}
