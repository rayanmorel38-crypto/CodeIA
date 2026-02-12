#include <iostream>
#include <string>
#include "utils/logger.h"

namespace cpp_engine { namespace modules { namespace security {

class SecurityModule {
public:
    static std::string name() { return "security_module"; }
    static void initialize() { cpp_engine::utils::Logger::instance().info("[SecurityModule] initialize"); }
    static void shutdown() { cpp_engine::utils::Logger::instance().info("[SecurityModule] shutdown"); }
};

} } }

extern "C" void register_module() {
    cpp_engine::modules::security::SecurityModule::initialize();
}
