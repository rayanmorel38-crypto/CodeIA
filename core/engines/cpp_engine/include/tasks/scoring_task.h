#pragma once
/**
 * @class ScoringTask
 * @brief Tâche de scoring (notation).
 */
#include <vector>
#include <string>
#include <stdexcept>

class ScoringTask {
public:
    ScoringTask();
    ~ScoringTask();
    void configure(const std::string& metric);
    std::vector<float> run(const std::vector<std::string>& items);
    void log(const std::string& message) const;
private:
    std::string metric_;
    bool configured_;
};
