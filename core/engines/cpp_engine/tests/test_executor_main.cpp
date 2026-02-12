#include "test_framework.h"
#include "../include/executor.h"
#include <fstream>
#include <cstdlib>
#include <unistd.h>
#include <sys/stat.h>
#include <iostream>
#include <chrono>
#include <thread>

// Helper to write test source files
static void write_hello_c(const std::string& path) {
    std::ofstream f(path);
    f << "#include <stdio.h>\n"
      << "int main() {\n"
      << "  printf(\"Hello from C\\n\");\n"
      << "  return 0;\n"
      << "}\n";
}

static void write_sleep_c(const std::string& path) {
    std::ofstream f(path);
    f << "#include <unistd.h>\n"
      << "int main() {\n"
      << "  sleep(10);\n"
      << "  return 0;\n"
      << "}\n";
}

static void write_exit_code_c(const std::string& path, int code) {
    std::ofstream f(path);
    f << "#include <stdlib.h>\n"
      << "int main() {\n"
      << "  return " << code << ";\n"
      << "}\n";
}

int main() {
    std::string tmpdir = "/tmp/cpp_engine_tests";
    mkdir(tmpdir.c_str(), 0777);

    // Test 1: Compile and run hello
    test::run_test("executor_compile_hello", []() {
        std::string tmpdir = "/tmp/cpp_engine_tests/hello";
        mkdir(tmpdir.c_str(), 0777);
        std::string src = tmpdir + "/hello.c";
        std::string bin = tmpdir + "/hello";
        write_hello_c(src);
        int rc = system(("gcc -o " + bin + " " + src).c_str());
        ASSERT_EQ(rc, 0);
        ASSERT_EQ(access(bin.c_str(), X_OK), 0);
        int exit_code = executor::runCommandWithLimits(bin, {}, 5, 5, 64 * 1024 * 1024);
        ASSERT_EQ(exit_code, 0);
    });

    // Test 2: Timeout kill
    test::run_test("executor_timeout_kill", []() {
        std::string tmpdir = "/tmp/cpp_engine_tests/timeout";
        mkdir(tmpdir.c_str(), 0777);
        std::string src = tmpdir + "/sleep.c";
        std::string bin = tmpdir + "/sleep";
        write_sleep_c(src);
        int rc = system(("gcc -o " + bin + " " + src).c_str());
        ASSERT_EQ(rc, 0);
        int exit_code = executor::runCommandWithLimits(bin, {}, 1, 10, 64 * 1024 * 1024);
        ASSERT_EQ(exit_code, -3); // Timeout kill code
    });

    // Test 3: Exit code 0
    test::run_test("executor_exit_code_0", []() {
        std::string tmpdir = "/tmp/cpp_engine_tests/exit0";
        mkdir(tmpdir.c_str(), 0777);
        std::string src = tmpdir + "/exit0.c";
        std::string bin = tmpdir + "/exit0";
        write_exit_code_c(src, 0);
        system(("gcc -o " + bin + " " + src).c_str());
        int exit_code = executor::runCommandWithLimits(bin, {}, 0, 5, 64 * 1024 * 1024);
        ASSERT_EQ(exit_code, 0);
    });

    // Test 4: Exit code 42
    test::run_test("executor_exit_code_42", []() {
        std::string tmpdir = "/tmp/cpp_engine_tests/exit42";
        mkdir(tmpdir.c_str(), 0777);
        std::string src = tmpdir + "/exit42.c";
        std::string bin = tmpdir + "/exit42";
        write_exit_code_c(src, 42);
        system(("gcc -o " + bin + " " + src).c_str());
        int exit_code = executor::runCommandWithLimits(bin, {}, 0, 5, 64 * 1024 * 1024);
        ASSERT_EQ(exit_code, 42);
    });

    // Test 5: Program with arguments
    test::run_test("executor_with_arguments", []() {
        std::string tmpdir = "/tmp/cpp_engine_tests/args";
        mkdir(tmpdir.c_str(), 0777);
        std::string src = tmpdir + "/echo_args.c";
        std::string bin = tmpdir + "/echo_args";
        std::ofstream f(src);
        f << "#include <stdio.h>\n"
          << "int main(int argc, char* argv[]) {\n"
          << "  for (int i = 1; i < argc; ++i) printf(\"%s\\n\", argv[i]);\n"
          << "  return 0;\n"
          << "}\n";
        f.close();
        system(("gcc -o " + bin + " " + src).c_str());
        int exit_code = executor::runCommandWithLimits(bin, {"arg1", "arg2", "arg3"}, 0, 5, 64 * 1024 * 1024);
        ASSERT_EQ(exit_code, 0);
    });

    // Test 6: compileAndRun convenience
    test::run_test("executor_compile_and_run", []() {
        std::string tmpdir = "/tmp/cpp_engine_tests/compileandrun";
        mkdir(tmpdir.c_str(), 0777);
        std::string src = tmpdir + "/simple.c";
        std::string bin = tmpdir + "/simple";
        write_hello_c(src);
        std::string compile = "gcc -o " + bin + " " + src;
        std::string run = bin;
        int exit_code = executor::compileAndRun(compile, run, 5, 5, 64 * 1024 * 1024);
        ASSERT_EQ(exit_code, 0);
    });

    // Test 7: No timeout (0 seconds)
    test::run_test("executor_no_timeout", []() {
        std::string tmpdir = "/tmp/cpp_engine_tests/notimeout";
        mkdir(tmpdir.c_str(), 0777);
        std::string src = tmpdir + "/quick.c";
        std::string bin = tmpdir + "/quick";
        write_exit_code_c(src, 0);
        system(("gcc -o " + bin + " " + src).c_str());
        int exit_code = executor::runCommandWithLimits(bin, {}, 0, 5, 64 * 1024 * 1024);
        ASSERT_EQ(exit_code, 0);
    });

    test::print_summary();
    return 0;
}
