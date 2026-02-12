#pragma once
/**
 * @class CodeGenerationTask
 * @brief Génération automatique de code source.
 */
#include <string>
#include <stdexcept>

class CodeGenerationTask {
public:
    CodeGenerationTask();
    ~CodeGenerationTask();
    void configure(const std::string& language);
    std::string run(const std::string& prompt);
    void log(const std::string& message) const;
private:
    std::string language_;
    bool configured_;
};
