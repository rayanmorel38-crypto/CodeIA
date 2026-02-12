#pragma once
/**
 * @class AutoCorrectionTask
 * @brief Correction automatique de texte ou code.
 */
#include <string>
#include <stdexcept>

class AutoCorrectionTask {
public:
    AutoCorrectionTask();
    ~AutoCorrectionTask();
    void configure(const std::string& mode);
    std::string run(const std::string& input);
    void log(const std::string& message) const;
private:
    std::string mode_;
    bool configured_;
};
