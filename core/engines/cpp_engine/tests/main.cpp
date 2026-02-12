// tests/main.cpp
#define CATCH_CONFIG_RUNNER
#include <catch2/catch_all.hpp>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <csignal>
#include <cstdlib>

namespace fs = std::filesystem;

static fs::path g_logs_dir;

static void setup_environment() {
    fs::path current = fs::current_path();
    if (current.filename() == "tests") {
        current = current.parent_path();
    }

    g_logs_dir = current / "logs";
    if (!fs::exists(g_logs_dir)) {
        fs::create_directories(g_logs_dir);
        std::cout << "[setup] Created logs dir: " << g_logs_dir << "\n";
    }

#if defined(_WIN32)
    _putenv_s("CPP_ENGINE_TESTING", "1");
#else
    setenv("CPP_ENGINE_TESTING", "1", 1);
#endif

    std::ofstream ofs((g_logs_dir / "tests_init.log").string(), std::ios::app);
    ofs << "[tests] init\n";
    ofs.close();
}

static void teardown_environment(int exit_code) {
    try {
        std::ofstream ofs((g_logs_dir / "tests_end.log").string(), std::ios::app);
        ofs << "[tests] finished with code " << exit_code << "\n";
        ofs.close();
    } catch (...) { }
}

int main(int argc, char* argv[]) {
    try { setup_environment(); } 
    catch (const std::exception &e) {
        std::cerr << "[setup] error: " << e.what() << "\n";
        return EXIT_FAILURE;
    }

    // Handle Ctrl+C gracefully
    std::signal(SIGINT, [](int){ 
        std::cerr << "[signal] SIGINT received, exiting.\n"; 
        std::exit(130); 
    });

    Catch::Session session;

    // Parse command-line args
    int returnCode = session.applyCommandLine(argc, argv);
    if (returnCode != 0) {
        teardown_environment(returnCode);
        return returnCode;
    }

    int result = session.run();
    teardown_environment(result);
    return (result < 0xff ? result : 255);
}

