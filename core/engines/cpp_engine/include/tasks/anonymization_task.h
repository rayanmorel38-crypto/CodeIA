#pragma once
/**
 * @class AnonymizationTask
 * @brief Anonymisation de données sensibles.
 */
#include <string>
#include <stdexcept>

class AnonymizationTask {
public:
    AnonymizationTask();
    ~AnonymizationTask();
    void configure(const std::string& method);
    std::string run(const std::string& input);
    void log(const std::string& message) const;
private:
    std::string method_;
    bool configured_;
};
