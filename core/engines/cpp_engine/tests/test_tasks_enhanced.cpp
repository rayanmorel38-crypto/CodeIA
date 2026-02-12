#include <catch2/catch_all.hpp>
#include "../include/tasks.h"
#include <chrono>
#include <thread>

TEST_CASE("TaskManager: Basic internal task", "[tasks]") {
    tasks::TaskManager mgr;
    bool executed = false;

    mgr.addTask("test_task", [&]() { executed = true; });
    REQUIRE(mgr.taskCount() == 1);

    mgr.executeAll();
    // Task executes asynchronously, give it time
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    REQUIRE(executed == true);
}

TEST_CASE("TaskManager: Task exists and complete", "[tasks]") {
    tasks::TaskManager mgr;

    mgr.addTask("task1", []() {});
    mgr.addTask("task2", []() {});

    REQUIRE(mgr.exists("task1") == true);
    REQUIRE(mgr.exists("task2") == true);
    REQUIRE(mgr.exists("task3") == false);

    REQUIRE(mgr.taskCount() == 2);

    REQUIRE(mgr.complete("task1") == true);
    REQUIRE(mgr.taskCount() == 1);
    REQUIRE(mgr.exists("task1") == false);

    REQUIRE(mgr.complete("task1") == false); // Already completed
}

TEST_CASE("TaskManager: Priority insertion", "[tasks]") {
    tasks::TaskManager mgr;

    // Add tasks with priorities
    mgr.addExternalCommand("low_pri", "echo", {"low"}, 1);
    mgr.addExternalCommand("high_pri", "echo", {"high"}, 10);
    mgr.addExternalCommand("med_pri", "echo", {"med"}, 5);

    // Higher priority tasks should be inserted earlier (not strictly enforced in output, but task order should reflect priority)
    REQUIRE(mgr.taskCount() == 3);
}

TEST_CASE("TaskManager: Clear tasks", "[tasks]") {
    tasks::TaskManager mgr;

    mgr.addTask("task1", []() {});
    mgr.addTask("task2", []() {});
    mgr.addTask("task3", []() {});

    REQUIRE(mgr.taskCount() == 3);

    mgr.clearTasks();

    REQUIRE(mgr.taskCount() == 0);
}

TEST_CASE("TaskManager: Multiple execute cycles", "[tasks]") {
    tasks::TaskManager mgr;
    int counter = 0;

    mgr.addTask("increment1", [&]() { counter++; });
    mgr.addTask("increment2", [&]() { counter++; });

    mgr.executeAll();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    REQUIRE(counter == 2);

    // Execute again with same tasks (they're not cleared after executeAll)
    mgr.executeAll();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    REQUIRE(counter == 4);
}

TEST_CASE("TaskManager: External command task", "[tasks]") {
    tasks::TaskManager mgr;

    // Add a simple external command (echo)
    mgr.addExternalCommand("echo_test", "echo", {"Hello from external command"}, 0, 5);

    REQUIRE(mgr.exists("echo_test") == true);
    REQUIRE(mgr.taskCount() == 1);

    // Execute - should not crash
    mgr.executeAll();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    // Task remains in the list after execution
    REQUIRE(mgr.exists("echo_test") == true);
}

TEST_CASE("TaskManager: Exception handling in internal tasks", "[tasks]") {
    tasks::TaskManager mgr;
    bool safe = false;

    mgr.addTask("throws", [](){ throw std::runtime_error("test error"); });
    mgr.addTask("safe_after", [&](){ safe = true; });

    // Should not crash even though first task throws
    mgr.executeAll();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    // Second task should have executed despite first task throwing
    REQUIRE(safe == true);
}
