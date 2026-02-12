#pragma once
#include <string>
#include <stdexcept>

/**
 * @class AnalysisTask
 * @brief Tâche d'analyse (statistiques / extraction simple).
 */
class AnalysisTask {
public:
    AnalysisTask();
    ~AnalysisTask();
    void configure(const std::string& mode = "basic");
    std::string run(const std::string& dataPath);
    void log(const std::string& message) const;
private:
    std::string mode_;
    bool configured_;
};
