#include <iostream>
#include <string>
#include <regex>
#include "utils/logger.h"

namespace cpp_engine { namespace modules { namespace security {

class Anonymizer {
public:
    static std::string name() { return "anonymizer"; }

    // Masks emails and digits sequences
    static std::string anonymize(const std::string &data) {
        std::string out = data;
        try {
            std::regex email_re(R"((\w+)(@)([\w\.]+))");
            out = std::regex_replace(out, email_re, "[email]");
            std::regex phone_re(R"(\+?\d[\d\-\s]{6,}\d)");
            out = std::regex_replace(out, phone_re, "[phone]");
        } catch (...) {}
        cpp_engine::utils::Logger::instance().info(std::string("[Anonymizer] -> ") + out);
        return out;
    }
};

} } }

extern "C" void register_module() {}
