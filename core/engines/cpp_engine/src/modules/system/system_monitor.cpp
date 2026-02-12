#include "modules/system/system_monitor.h"
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <fstream>
#include <sstream>
#include <mutex>
#include <random>
#include "utils/logger.h"

namespace cppengine { namespace modules { namespace system {

SystemMonitor::SystemMonitor() : monitoring_(false), cpu_threshold_(80.0), memory_threshold_(1024.0) {}

SystemMonitor::~SystemMonitor() {
    shutdown();
}

SystemMonitor& SystemMonitor::instance() {
    static SystemMonitor instance;
    return instance;
}

void SystemMonitor::init() {
    if (monitoring_) return;
    monitoring_ = true;
    monitor_thread_ = std::thread(&SystemMonitor::monitor_loop, this);
    cpp_engine::utils::Logger::instance().info("[SystemMonitor] initialized");
}

void SystemMonitor::shutdown() {
    if (!monitoring_) return;
    monitoring_ = false;
    if (monitor_thread_.joinable()) {
        monitor_thread_.join();
    }
    cpp_engine::utils::Logger::instance().info("[SystemMonitor] shutdown");
}

SystemMetrics SystemMonitor::get_current_metrics() {
    // Generate mock metrics for demo
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<> cpu_dist(10.0, 90.0);
    static std::uniform_real_distribution<> mem_dist(256.0, 2048.0);
    static std::uniform_real_distribution<> disk_dist(20.0, 95.0);
    static std::uniform_int_distribution<> proc_dist(50, 200);
    
    SystemMetrics metrics;
    metrics.cpu_usage_percent = cpu_dist(gen);
    metrics.memory_usage_mb = mem_dist(gen);
    metrics.disk_usage_percent = disk_dist(gen);
    metrics.active_processes = proc_dist(gen);
    metrics.timestamp = std::chrono::system_clock::now();
    
    std::lock_guard<std::mutex> lock(mutex_);
    metrics_history_.push_back(metrics);
    if (metrics_history_.size() > 100) {
        metrics_history_.erase(metrics_history_.begin());
    }
    
    return metrics;
}

std::vector<SystemMetrics> SystemMonitor::get_metrics_history() {
    std::lock_guard<std::mutex> lock(mutex_);
    return metrics_history_;
}

void SystemMonitor::set_alert_thresholds(double cpu_threshold, double memory_threshold_mb) {
    cpu_threshold_ = cpu_threshold;
    memory_threshold_ = memory_threshold_mb;
    cpp_engine::utils::Logger::instance().info("[SystemMonitor] alert thresholds updated");
}

void SystemMonitor::monitor_loop() {
    while (monitoring_) {
        get_current_metrics();
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}

} } }