// Simple test framework (no external dependencies)
#pragma once
#include <string>
#include <iostream>
#include <functional>
#include <vector>
#include <cstdlib>

namespace test {

struct TestResult {
    std::string name;
    bool passed;
    std::string message;
};

static std::vector<TestResult> results;

class AssertionError : public std::exception {
public:
    explicit AssertionError(const std::string& msg) : msg_(msg) {}
    const char* what() const noexcept override { return msg_.c_str(); }
private:
    std::string msg_;
};

#define ASSERT_EQ(a, b) do { if ((a) != (b)) throw test::AssertionError(std::string(__FILE__) + ":" + std::to_string(__LINE__) + ": " + #a + " != " + #b); } while(0)
#define ASSERT_NE(a, b) do { if ((a) == (b)) throw test::AssertionError(std::string(__FILE__) + ":" + std::to_string(__LINE__) + ": " + #a + " == " + #b); } while(0)
#define ASSERT_TRUE(a) do { if (!(a)) throw test::AssertionError(std::string(__FILE__) + ":" + std::to_string(__LINE__) + ": " + #a + " is false"); } while(0)
#define ASSERT_FALSE(a) do { if ((a)) throw test::AssertionError(std::string(__FILE__) + ":" + std::to_string(__LINE__) + ": " + #a + " is true"); } while(0)

void run_test(const std::string& name, std::function<void()> fn) {
    try {
        fn();
        results.push_back({name, true, ""});
        std::cout << "✓ " << name << "\n";
    } catch (const AssertionError& e) {
        results.push_back({name, false, e.what()});
        std::cout << "✗ " << name << " - " << e.what() << "\n";
    } catch (const std::exception& e) {
        results.push_back({name, false, std::string(e.what())});
        std::cout << "✗ " << name << " - Exception: " << e.what() << "\n";
    } catch (...) {
        results.push_back({name, false, "Unknown exception"});
        std::cout << "✗ " << name << " - Unknown exception\n";
    }
}

void print_summary() {
    int passed = 0, failed = 0;
    for (const auto& r : results) {
        if (r.passed) passed++;
        else failed++;
    }
    std::cout << "\n" << passed << " passed, " << failed << " failed out of " << results.size() << " tests\n";
    exit(failed > 0 ? 1 : 0);
}

} // namespace test
