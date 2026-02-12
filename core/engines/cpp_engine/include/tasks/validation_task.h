#pragma once
/**
 * @class ValidationTask
 * @brief Tâche de validation de données ou modèles.
 */
#include <string>
#include <stdexcept>

class ValidationTask {
public:
    ValidationTask();
    ~ValidationTask();
    void configure(const std::string& type);
    bool run(const std::string& input);
    void log(const std::string& message) const;
private:
    std::string type_;
    bool configured_;
};
