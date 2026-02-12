#pragma once
#include <string>
#include <stdexcept>

/**
 * @class SpeechRecognitionTask
 * @brief Reconnaissance vocale (speech-to-text) - simulation.
 */
class SpeechRecognitionTask {
public:
    SpeechRecognitionTask();
    ~SpeechRecognitionTask();
    void configure(const std::string& model = "");
    std::string run(const std::string& audioPath);
    void log(const std::string& message) const;
private:
    std::string model_;
    bool configured_;
};
