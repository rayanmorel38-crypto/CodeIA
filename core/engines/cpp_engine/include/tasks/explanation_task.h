#pragma once
/**
 * @class ExplanationTask
 * @brief Tâche d'explication de décision IA.
 */
#include <string>
#include <stdexcept>

class ExplanationTask {
public:
    ExplanationTask();
    ~ExplanationTask();
    void configure(const std::string& method);
    std::string run(const std::string& input);
    void log(const std::string& message) const;
private:
    std::string method_;
    bool configured_;
};
