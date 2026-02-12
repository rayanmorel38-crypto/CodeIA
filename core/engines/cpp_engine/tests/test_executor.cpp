#include <catch2/catch_all.hpp>
#include "../include/executor.h"
#include <fstream>
#include <cstdlib>
#include <unistd.h>
#include <sys/stat.h>

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
      << "  sleep(10); /* Will be killed by timeout */\n"
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

TEST_CASE("Executor: Compile and run hello", "[executor]") {
    std::string tmpdir = "/tmp/cpp_engine_test_hello";
    mkdir(tmpdir.c_str(), 0777);
    std::string src = tmpdir + "/hello.c";
    std::string bin = tmpdir + "/hello";

    write_hello_c(src);

    // Compile
    std::string compile_cmd = "gcc -o " + bin + " " + src;
    int rc = system(compile_cmd.c_str());
    REQUIRE(rc == 0);
    REQUIRE(access(bin.c_str(), X_OK) == 0);

    // Run via executor
    int exit_code = executor::runCommandWithLimits(bin, {}, 5, 5, 64 * 1024 * 1024);
    REQUIRE(exit_code == 0);
}

TEST_CASE("Executor: Timeout kill", "[executor]") {
    std::string tmpdir = "/tmp/cpp_engine_test_timeout";
    mkdir(tmpdir.c_str(), 0777);
    std::string src = tmpdir + "/sleep.c";
    std::string bin = tmpdir + "/sleep";

    write_sleep_c(src);

    // Compile
    std::string compile_cmd = "gcc -o " + bin + " " + src;
    int rc = system(compile_cmd.c_str());
    REQUIRE(rc == 0);

    // Run with short timeout — should be killed
    int exit_code = executor::runCommandWithLimits(bin, {}, 1, 10, 64 * 1024 * 1024);
    REQUIRE(exit_code == -3); // Timeout kill code
}

TEST_CASE("Executor: Exit code preservation", "[executor]") {
    std::string tmpdir = "/tmp/cpp_engine_test_exitcode";
    mkdir(tmpdir.c_str(), 0777);

    SECTION("Exit code 0") {
        std::string src = tmpdir + "/exit0.c";
        std::string bin = tmpdir + "/exit0";
        write_exit_code_c(src, 0);
        system(("gcc -o " + bin + " " + src).c_str());
        REQUIRE(executor::runCommandWithLimits(bin, {}, 0, 5, 64 * 1024 * 1024) == 0);
    }

    SECTION("Exit code 42") {
        std::string src = tmpdir + "/exit42.c";
        std::string bin = tmpdir + "/exit42";
        write_exit_code_c(src, 42);
        system(("gcc -o " + bin + " " + src).c_str());
        REQUIRE(executor::runCommandWithLimits(bin, {}, 0, 5, 64 * 1024 * 1024) == 42);
    }
}

TEST_CASE("Executor: Program with arguments", "[executor]") {
    std::string tmpdir = "/tmp/cpp_engine_test_args";
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
    REQUIRE(exit_code == 0);
}

TEST_CASE("Executor: compileAndRun convenience", "[executor]") {
    std::string tmpdir = "/tmp/cpp_engine_test_compileandrun";
    mkdir(tmpdir.c_str(), 0777);
    std::string src = tmpdir + "/simple.c";
    std::string bin = tmpdir + "/simple";

    write_hello_c(src);

    std::string compile = "gcc -o " + bin + " " + src;
    std::string run = bin;

    int exit_code = executor::compileAndRun(compile, run, 5, 5, 64 * 1024 * 1024);
    REQUIRE(exit_code == 0);
}

TEST_CASE("Executor: No timeout (0 seconds)", "[executor]") {
    std::string tmpdir = "/tmp/cpp_engine_test_notimeout";
    mkdir(tmpdir.c_str(), 0777);
    std::string src = tmpdir + "/quick.c";
    std::string bin = tmpdir + "/quick";

    write_exit_code_c(src, 0);
    system(("gcc -o " + bin + " " + src).c_str());

    // timeout_seconds = 0 means no timeout
    int exit_code = executor::runCommandWithLimits(bin, {}, 0, 5, 64 * 1024 * 1024);
    REQUIRE(exit_code == 0);
}
