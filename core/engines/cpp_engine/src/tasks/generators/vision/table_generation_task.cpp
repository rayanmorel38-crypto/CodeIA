#include "../../include/tasks/table_generation_task.h"
#include <iostream>
#include "utils/logger.h"

TableGenerationTask::TableGenerationTask() : format_("csv"), configured_(false) {}
TableGenerationTask::~TableGenerationTask() {}

void TableGenerationTask::configure(const std::string& format) {
    format_ = format;
    configured_ = true;
    log("Format: " + format_);
}

std::string TableGenerationTask::run(const std::string& prompt) {
    if (!configured_) throw std::runtime_error("TableGenerationTask non configurée");
    log("Generating table for: " + prompt);
    // Simulated CSV
    std::string out = "col1,col2\nval1,val2\n";
    return out;
}

void TableGenerationTask::log(const std::string& message) const {
    cpp_engine::utils::Logger::instance().info("[TableGenerationTask] " + message);
}
