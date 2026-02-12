#pragma once
#include <string>
#include <stdexcept>

/**
 * @class TranslationTask
 * @brief Traduction automatique (simulation).
 */
class TranslationTask {
public:
    TranslationTask();
    ~TranslationTask();
    void configure(const std::string& targetLang = "en");
    std::string run(const std::string& text);
    void log(const std::string& message) const;
private:
    std::string targetLang_;
    bool configured_;
};
