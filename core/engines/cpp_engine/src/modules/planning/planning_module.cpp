#include <iostream>
#include <string>
#include "utils/logger.h"

namespace cpp_engine { namespace modules { namespace planning {

class PlanningModule {
public:
    static std::string name() { return "planning_module"; }
    static void initialize() { cpp_engine::utils::Logger::instance().info("[PlanningModule] initialize"); }
    static void shutdown() { cpp_engine::utils::Logger::instance().info("[PlanningModule] shutdown"); }
};

} } }

extern "C" void register_module() {
    cpp_engine::modules::planning::PlanningModule::initialize();
}
