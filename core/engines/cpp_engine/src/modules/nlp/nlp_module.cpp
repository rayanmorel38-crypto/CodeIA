#include <iostream>
#include <string>
#include "utils/logger.h"

namespace cpp_engine { namespace modules { namespace nlp {

class NLPModule {
public:
    static std::string name() { return "nlp_module"; }
    static void initialize() { cpp_engine::utils::Logger::instance().info("[NLPModule] initialize"); }
    static void shutdown() { cpp_engine::utils::Logger::instance().info("[NLPModule] shutdown"); }
};

} } }

extern "C" void register_module() {
    cpp_engine::modules::nlp::NLPModule::initialize();
}
