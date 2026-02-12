#ifndef TASKS_H
#define TASKS_H

#include <string>
#include <vector>
#include <functional>

// Inclusion centralisée de toutes les tâches IA
#include "object_detection_task.h"
#include "feature_extraction_task.h"
#include "image3d_generation_task.h"
#include "simulation_task.h"
#include "recommendation_task.h"
#include "search_task.h"
#include "summarization_task.h"
#include "code_generation_task.h"
#include "auto_correction_task.h"
#include "anonymization_task.h"
#include "graph_generation_task.h"
#include "clustering_task.h"
#include "scoring_task.h"
#include "calibration_task.h"
#include "monitoring_task.h"
#include "user_feedback_task.h"
#include "logging_task.h"
#include "security_task.h"
#include "validation_task.h"
#include "explanation_task.h"
// Newly added high-performance tasks
#include "compression_task.h"
#include "decompression_task.h"
#include "gpu_acceleration_task.h"
#include "tensor_inference_task.h"
#include "physics_simulation_task.h"
#include "rendering_task.h"
#include "realtime_streaming_task.h"
#include "hashing_task.h"
#include "graph_optimization_task.h"
#include "lowlatency_audio_task.h"

namespace tasks {

struct Task {
    enum class Type { INTERNAL, EXTERNAL_COMMAND };
    std::string name;
    std::function<void()> action; // used for INTERNAL tasks
    // For EXTERNAL_COMMAND tasks, provide command and args
    std::string command;
    std::vector<std::string> args;
    Type type = Type::INTERNAL;
    int priority = 0; // higher => run earlier
    unsigned int timeout_seconds = 0; // 0 = no timeout
};

class TaskManager {
public:
    TaskManager();
    ~TaskManager();

    void addTask(const std::string& name, std::function<void()> action);
    // Add task with optional priority (higher => executed earlier)
    void addTaskWithPriority(const std::string& name, std::function<void()> action, int priority = 0);
    // Add external command task
    void addExternalCommand(const std::string& name, const std::string& command, const std::vector<std::string>& args, int priority = 0, unsigned int timeout_seconds = 0);
    void executeAll();
    void clearTasks();
    bool exists(const std::string& name) const;
    bool complete(const std::string& name);
    size_t taskCount() const;

private:
    std::vector<Task> tasks_;
};

} // namespace tasks

// C-style compatibility API used by legacy tests and scripts.
// These functions are thin wrappers around a singleton TaskManager internally.
extern "C" {
    void init_tasks();
    bool add_task(const char* name);
    size_t get_task_count();
    bool task_exists(const char* name);
    bool complete_task(const char* name);
}

#endif

