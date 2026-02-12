#ifndef EXECUTOR_H
#define EXECUTOR_H

#include <string>
#include <vector>

namespace executor {
    // Run a command (path or program) with args, enforce timeout in seconds (0 = no timeout).
    // Returns exit code (>=0) or -1 for failed to fork/exec, -2 for killed by signal, -3 for timeout kill.
    int runCommandWithLimits(const std::string& command, const std::vector<std::string>& args, unsigned int timeout_seconds = 0, unsigned int cpu_limit_seconds = 5, size_t memory_limit_bytes = 64 * 1024 * 1024, bool drop_privileges = false, const std::string& chroot_dir = "");

    // Convenience: build and run a compiled program (compile_cmd is the full command to compile source)
    int compileAndRun(const std::string& compile_cmd, const std::string& run_cmd, unsigned int timeout_seconds = 0, unsigned int cpu_limit_seconds = 5, size_t memory_limit_bytes = 64 * 1024 * 1024, bool drop_privileges = false, const std::string& chroot_dir = "");
}

#endif
