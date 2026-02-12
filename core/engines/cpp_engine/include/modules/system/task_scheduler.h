#ifndef CPP_ENGINE_MODULES_SYSTEM_TASK_SCHEDULER_H
#define CPP_ENGINE_MODULES_SYSTEM_TASK_SCHEDULER_H

#include <string>
#include <chrono>
#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <vector>

namespace cppengine { namespace modules { namespace system {

struct ScheduledTask {
    std::string id;
    std::chrono::system_clock::time_point execute_at;
    std::function<void()> action;
    bool recurring;
    std::chrono::seconds interval;
};

class TaskScheduler {
public:
    static TaskScheduler& instance();
    void init();
    void shutdown();
    void schedule_once(const std::string& id, std::chrono::seconds delay, std::function<void()> action);
    void schedule_recurring(const std::string& id, std::chrono::seconds interval, std::function<void()> action);
    void cancel_task(const std::string& id);

private:
    TaskScheduler();
    ~TaskScheduler();
    
    void run_scheduler();
    
    std::thread scheduler_thread_;
    std::mutex mutex_;
    std::condition_variable cv_;
    std::priority_queue<ScheduledTask, std::vector<ScheduledTask>, std::greater<ScheduledTask>> tasks_;
    bool running_;
};

} } }

#endif // CPP_ENGINE_MODULES_SYSTEM_TASK_SCHEDULER_H