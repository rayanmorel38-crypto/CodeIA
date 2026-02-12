#pragma once
/**
 * @class SummarizationTask
 * @brief Résumé automatique de texte ou données.
 */
#include <string>
#include <stdexcept>

class SummarizationTask {
public:
    SummarizationTask();
    ~SummarizationTask();
    void configure(const std::string& method);
    std::string run(const std::string& input);
    void log(const std::string& message) const;
private:
    std::string method_;
    bool configured_;
};
