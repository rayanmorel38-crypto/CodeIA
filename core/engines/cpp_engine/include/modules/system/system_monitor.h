#ifndef CPP_ENGINE_MODULES_SYSTEM_SYSTEM_MONITOR_H
#define CPP_ENGINE_MODULES_SYSTEM_SYSTEM_MONITOR_H

#include <vector>
#include <chrono>
#include <thread>
#include <mutex>

namespace cppengine { namespace modules { namespace system {

struct SystemMetrics {
    double cpu_usage_percent;
    double memory_usage_mb;
    double disk_usage_percent;
    int active_processes;
    std::chrono::system_clock::time_point timestamp;
};

class SystemMonitor {
public:
    static SystemMonitor& instance();
    void init();
    void shutdown();
    SystemMetrics get_current_metrics();
    std::vector<SystemMetrics> get_metrics_history();
    void set_alert_thresholds(double cpu_threshold, double memory_threshold_mb);

private:
    SystemMonitor();
    ~SystemMonitor();
    
    void monitor_loop();
    
    std::thread monitor_thread_;
    std::mutex mutex_;
    std::vector<SystemMetrics> metrics_history_;
    bool monitoring_;
    double cpu_threshold_;
    double memory_threshold_;
};

} } }

#endif // CPP_ENGINE_MODULES_SYSTEM_SYSTEM_MONITOR_H