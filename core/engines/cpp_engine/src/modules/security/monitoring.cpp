#include <iostream>
#include <string>
#include <unordered_map>
#include <mutex>
#include "utils/logger.h"

namespace cpp_engine { namespace modules { namespace security {

class Monitoring {
public:
    static std::string name() { return "monitoring"; }
    static void watch(const std::string &subject) {
        std::lock_guard<std::mutex> lk(mtx_);
        ++counters_[subject];
        cpp_engine::utils::Logger::instance().info(std::string("[Monitoring] ") + subject + " count=" + std::to_string(counters_[subject]));
    }

    static int get_count(const std::string &subject) {
        std::lock_guard<std::mutex> lk(mtx_);
        return counters_[subject];
    }

private:
    static inline std::unordered_map<std::string,int> counters_{};
    static inline std::mutex mtx_;
};

} } }

extern "C" void register_module() {}
