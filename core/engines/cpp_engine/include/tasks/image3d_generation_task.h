#pragma once
/**
 * @class Image3DGenerationTask
 * @brief Génération d'images 3D.
 */
#include <string>
#include <stdexcept>

class Image3DGenerationTask {
public:
    Image3DGenerationTask();
    ~Image3DGenerationTask();
    void configure(const std::string& modelPath);
    bool run(const std::string& outputPath);
    void log(const std::string& message) const;
private:
    std::string modelPath_;
    bool configured_;
};
