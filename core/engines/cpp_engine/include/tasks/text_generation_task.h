#pragma once
#include <string>
#include <stdexcept>

/**
 * @class TextGenerationTask
 * @brief Génération de texte (LLM wrapper simulation).
 */
class TextGenerationTask {
public:
    TextGenerationTask();
    ~TextGenerationTask();
    void configure(const std::string& model = "");
    std::string run(const std::string& prompt);
    void log(const std::string& message) const;
private:
    std::string model_;
    bool configured_;
};
