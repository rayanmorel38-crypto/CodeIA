#include <iostream>
#include <string>
#include "utils/logger.h"

namespace cpp_engine { namespace modules { namespace planning {

class Simulator {
public:
    static std::string name() { return "simulator"; }
    static void simulate(const std::string &scenario) {
        cpp_engine::utils::Logger::instance().info(std::string("[Simulator] simulate: ") + scenario);
    }
};

} } }

extern "C" void register_module() {
    cpp_engine::modules::planning::Simulator::simulate("<scenario>");
}
