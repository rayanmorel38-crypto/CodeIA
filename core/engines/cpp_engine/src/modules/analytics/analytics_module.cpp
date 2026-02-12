#include <iostream>
#include <string>

namespace cpp_engine { namespace modules { namespace analytics {

class AnalyticsModule {
public:
    static std::string name() { return "analytics_module"; }
    static void initialize() { std::cout << "[AnalyticsModule] initialize\n"; }
    static void shutdown() { std::cout << "[AnalyticsModule] shutdown\n"; }
};

} } }

extern "C" void register_module() {
    cpp_engine::modules::analytics::AnalyticsModule::initialize();
}
