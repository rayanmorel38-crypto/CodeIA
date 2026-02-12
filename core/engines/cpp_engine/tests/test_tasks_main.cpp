#include "test_framework.h"
#include "../include/tasks.h"
#include <chrono>
#include <thread>
#include <iostream>

int main() {
    // Test 1: Basic internal task
    test::run_test("tasks_basic_internal_task", []() {
        tasks::TaskManager mgr;
        bool executed = false;
        mgr.addTask("test_task", [&]() { executed = true; });
        ASSERT_EQ(mgr.taskCount(), 1);
        mgr.executeAll();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        ASSERT_TRUE(executed);
    });

    // Test 2: Task exists and complete
    test::run_test("tasks_exists_and_complete", []() {
        tasks::TaskManager mgr;
        mgr.addTask("task1", []() {});
        mgr.addTask("task2", []() {});
        ASSERT_TRUE(mgr.exists("task1"));
        ASSERT_TRUE(mgr.exists("task2"));
        ASSERT_FALSE(mgr.exists("task3"));
        ASSERT_EQ(mgr.taskCount(), 2);
        ASSERT_TRUE(mgr.complete("task1"));
        ASSERT_EQ(mgr.taskCount(), 1);
        ASSERT_FALSE(mgr.exists("task1"));
        ASSERT_FALSE(mgr.complete("task1")); // Already completed
    });

    // Test 3: Priority insertion
    test::run_test("tasks_priority_insertion", []() {
        tasks::TaskManager mgr;
        mgr.addExternalCommand("low_pri", "echo", {"low"}, 1);
        mgr.addExternalCommand("high_pri", "echo", {"high"}, 10);
        mgr.addExternalCommand("med_pri", "echo", {"med"}, 5);
        ASSERT_EQ(mgr.taskCount(), 3);
    });

    // Test 4: Clear tasks
    test::run_test("tasks_clear", []() {
        tasks::TaskManager mgr;
        mgr.addTask("task1", []() {});
        mgr.addTask("task2", []() {});
        mgr.addTask("task3", []() {});
        ASSERT_EQ(mgr.taskCount(), 3);
        mgr.clearTasks();
        ASSERT_EQ(mgr.taskCount(), 0);
    });

    // Test 5: Multiple execute cycles
    test::run_test("tasks_multiple_execute_cycles", []() {
        tasks::TaskManager mgr;
        int counter = 0;
        mgr.addTask("increment1", [&]() { counter++; });
        mgr.addTask("increment2", [&]() { counter++; });
        mgr.executeAll();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        ASSERT_EQ(counter, 2);
        mgr.executeAll();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        ASSERT_EQ(counter, 4);
    });

    // Test 6: External command task
    test::run_test("tasks_external_command", []() {
        tasks::TaskManager mgr;
        mgr.addExternalCommand("echo_test", "echo", {"Hello"}, 0, 5);
        ASSERT_TRUE(mgr.exists("echo_test"));
        ASSERT_EQ(mgr.taskCount(), 1);
        mgr.executeAll();
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        ASSERT_TRUE(mgr.exists("echo_test"));
    });

    // Test 7: Exception handling in internal tasks
    test::run_test("tasks_exception_handling", []() {
        tasks::TaskManager mgr;
        bool safe = false;
        mgr.addTask("throws", [](){ throw std::runtime_error("test error"); });
        mgr.addTask("safe_after", [&](){ safe = true; });
        mgr.executeAll();
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        ASSERT_TRUE(safe);
    });

    test::print_summary();
    return 0;
}
