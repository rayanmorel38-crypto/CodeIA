#include "modules/system/task_scheduler.h"
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <functional>
#include <mutex>
#include <queue>
#include <condition_variable>
#include "utils/logger.h"

namespace cppengine { namespace modules { namespace system {

bool operator>(const ScheduledTask& a, const ScheduledTask& b) {
    return a.execute_at > b.execute_at;
}

TaskScheduler::TaskScheduler() : running_(false) {}

TaskScheduler::~TaskScheduler() {
    shutdown();
}

TaskScheduler& TaskScheduler::instance() {
    static TaskScheduler instance;
    return instance;
}

void TaskScheduler::init() {
    if (running_) return;
    running_ = true;
    scheduler_thread_ = std::thread(&TaskScheduler::run_scheduler, this);
    cpp_engine::utils::Logger::instance().info("[TaskScheduler] initialized");
}

void TaskScheduler::shutdown() {
    if (!running_) return;
    running_ = false;
    cv_.notify_all();
    if (scheduler_thread_.joinable()) {
        scheduler_thread_.join();
    }
    cpp_engine::utils::Logger::instance().info("[TaskScheduler] shutdown");
}

void TaskScheduler::schedule_once(const std::string& id, std::chrono::seconds delay, std::function<void()> action) {
    std::lock_guard<std::mutex> lock(mutex_);
    ScheduledTask task{id, std::chrono::system_clock::now() + delay, action, false, std::chrono::seconds(0)};
    tasks_.push(task);
    cv_.notify_one();
    cpp_engine::utils::Logger::instance().info("[TaskScheduler] scheduled one-time task: " + id);
}

void TaskScheduler::schedule_recurring(const std::string& id, std::chrono::seconds interval, std::function<void()> action) {
    std::lock_guard<std::mutex> lock(mutex_);
    ScheduledTask task{id, std::chrono::system_clock::now() + interval, action, true, interval};
    tasks_.push(task);
    cv_.notify_one();
    cpp_engine::utils::Logger::instance().info("[TaskScheduler] scheduled recurring task: " + id);
}

void TaskScheduler::cancel_task(const std::string& id) {
    std::lock_guard<std::mutex> lock(mutex_);
    // In a real implementation, we'd need to track tasks by ID to cancel them
    cpp_engine::utils::Logger::instance().info("[TaskScheduler] task cancellation not implemented: " + id);
}

void TaskScheduler::run_scheduler() {
    while (running_) {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this]() { return !running_ || !tasks_.empty(); });
        
        if (!running_) break;
        
        auto now = std::chrono::system_clock::now();
        if (!tasks_.empty() && tasks_.top().execute_at <= now) {
            ScheduledTask task = tasks_.top();
            tasks_.pop();
            lock.unlock();
            
            try {
                task.action();
            } catch (const std::exception& e) {
                cpp_engine::utils::Logger::instance().error("[TaskScheduler] task execution failed: " + std::string(e.what()));
            }
            
            if (task.recurring) {
                lock.lock();
                task.execute_at = now + task.interval;
                tasks_.push(task);
            }
        }
    }
}

} } }