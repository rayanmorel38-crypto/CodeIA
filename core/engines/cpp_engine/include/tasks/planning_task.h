#pragma once
#include <string>
#include <stdexcept>

/**
 * @class PlanningTask
 * @brief Génération de plans/action sequences.
 */
class PlanningTask {
public:
    PlanningTask();
    ~PlanningTask();
    void configure(const std::string& strategy = "default");
    std::string run(const std::string& goal);
    void log(const std::string& message) const;
private:
    std::string strategy_;
    bool configured_;
};
