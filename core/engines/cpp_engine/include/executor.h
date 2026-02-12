// executor.h - simple declarations matching src/core/executor.cpp
#pragma once

#include <string>
#include <vector>

namespace executor {

int runCommandWithLimits(const std::string& command, const std::vector<std::string>& args, unsigned int timeout_seconds = 0, unsigned int cpu_limit_seconds = 0, size_t memory_limit_bytes = 0, bool drop_privileges = false, const std::string& chroot_dir = "");

int compileAndRun(const std::string& compile_cmd, const std::string& run_cmd, unsigned int timeout_seconds = 0, unsigned int cpu_limit_seconds = 0, size_t memory_limit_bytes = 0, bool drop_privileges = false, const std::string& chroot_dir = "");

} // namespace executor
