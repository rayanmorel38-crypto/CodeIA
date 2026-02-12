#include <iostream>
#include <string>
#include "utils/logger.h"

namespace cpp_engine { namespace modules { namespace security {

class Validation {
public:
    static std::string name() { return "validation"; }

    // Very naive JSON-like validator: checks balanced braces and brackets
    static bool validate(const std::string &payload) {
        int b = 0, sq = 0;
        for (char c : payload) {
            if (c == '{') ++b; else if (c == '}') --b;
            if (c == '[') ++sq; else if (c == ']') --sq;
            if (b < 0 || sq < 0) return false;
        }
        bool ok = (b == 0 && sq == 0);
        cpp_engine::utils::Logger::instance().info(std::string("[Validation] payload valid=") + (ok?"true":"false"));
        return ok;
    }
};

} } }

extern "C" void register_module() {}
