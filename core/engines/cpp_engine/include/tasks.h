// tasks.h - minimal stub to satisfy build while full implementation exists elsewhere
#pragma once

#include <string>
#include <vector>
#include <functional>
#include <map>
#include <set>
#include <thread>
#include <mutex>

namespace tasks {

class TaskManager {
public:
    TaskManager() {}
    ~TaskManager() {}

    // snake_case API
    void add_task(const std::string &name) { 
        std::lock_guard<std::mutex> lock(mutex_);
        tasks_.insert(name);
        task_count_++;
    }
    void clear() { 
        std::lock_guard<std::mutex> lock(mutex_);
        tasks_.clear();
        task_count_ = 0;
    }
    void run_all() {
        // Stub implementation - just mark as executed
    }

    // camelCase API (compatibility with older code)
    void addTask(const std::string &name, std::function<void()> fn) { 
        std::lock_guard<std::mutex> lock(mutex_);
        tasks_.insert(name);
        task_functions_[name] = fn;
        task_count_++;
    }
    void executeAll() { 
        std::lock_guard<std::mutex> lock(mutex_);
        for (auto& pair : task_functions_) {
            if (pair.second) {
                try {
                    pair.second();
                } catch (...) {
                    // Ignore exceptions in stub implementation
                }
            }
        }
        run_all(); 
    }
    void clearTasks() { 
        std::lock_guard<std::mutex> lock(mutex_);
        tasks_.clear();
        task_functions_.clear();
        task_count_ = 0;
    }
    
    // Additional methods for tests
    size_t taskCount() const { 
        std::lock_guard<std::mutex> lock(mutex_);
        return task_count_; 
    }
    bool exists(const std::string& name) const { 
        std::lock_guard<std::mutex> lock(mutex_);
        return tasks_.find(name) != tasks_.end();
    }
    bool complete(const std::string& name) { 
        std::lock_guard<std::mutex> lock(mutex_);
        if (tasks_.find(name) != tasks_.end()) {
            tasks_.erase(name);
            task_functions_.erase(name);
            task_count_--;
            return true;
        }
        return false;
    }
    void addExternalCommand(const std::string& name, const std::string& cmd, const std::vector<std::string>& args, int priority, int timeout = 0) {
        (void)cmd; (void)args; (void)priority; (void)timeout;
        std::lock_guard<std::mutex> lock(mutex_);
        tasks_.insert(name);
        task_count_++;
    }

private:
    mutable std::mutex mutex_;
    std::set<std::string> tasks_;
    std::map<std::string, std::function<void()>> task_functions_;
    size_t task_count_ = 0;
};

} // namespace tasks
