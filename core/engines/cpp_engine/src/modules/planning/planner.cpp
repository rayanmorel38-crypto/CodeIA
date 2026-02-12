#include <iostream>
#include <string>
#include "utils/logger.h"

namespace cpp_engine { namespace modules { namespace planning {

class Planner {
public:
    static std::string name() { return "planner"; }
    static std::string plan(const std::string &goal) {
        cpp_engine::utils::Logger::instance().info(std::string("[Planner] planning for: ") + goal);
        return "plan";
    }
};

} } }

extern "C" void register_module() {
    cpp_engine::modules::planning::Planner::plan("<goal>");
}
