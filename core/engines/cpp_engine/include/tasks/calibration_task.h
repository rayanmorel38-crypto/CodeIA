#pragma once
/**
 * @class CalibrationTask
 * @brief Tâche de calibration de modèles ou capteurs.
 */
#include <string>
#include <stdexcept>

class CalibrationTask {
public:
    CalibrationTask();
    ~CalibrationTask();
    void configure(const std::string& target);
    bool run();
    void log(const std::string& message) const;
private:
    std::string target_;
    bool configured_;
};
