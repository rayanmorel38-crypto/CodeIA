#ifndef CPP_ENGINE_H
#define CPP_ENGINE_H

#include "core.h"
#include "tasks.h"
#include "config.h"
#include "macros.h"
#include "utils.h"
#include <vector>
#include <string>

namespace cppengine {

class CppEngine {
public:
    CppEngine();
    ~CppEngine();

    void initialize();
    void shutdown();

    void loadTasks(const std::vector<std::string>& task_names);
    void runAllTasks();
    void clearTasks();

    core::Core& getCore();
    tasks::TaskManager& getTaskManager();
    config::Config& getConfig();

private:
    core::Core core_;
    tasks::TaskManager task_manager_;
    config::Config config_;
};

} // namespace cppengine

#endif

