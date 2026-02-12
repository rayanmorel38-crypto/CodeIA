#pragma once
/**
 * @class SecurityTask
 * @brief Tâche de sécurité et contrôle d'accès.
 */
#include <string>
#include <stdexcept>

class SecurityTask {
public:
    SecurityTask();
    ~SecurityTask();
    void configure(const std::string& policy);
    bool run(const std::string& resource);
    void log(const std::string& message) const;
private:
    std::string policy_;
    bool configured_;
};
