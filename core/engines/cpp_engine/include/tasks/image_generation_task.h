#pragma once
#include <string>
#include <vector>
#include <stdexcept>

/**
 * @class ImageGenerationTask
 * @brief Génération d'images à partir d'un prompt.
 */
class ImageGenerationTask {
public:
    ImageGenerationTask();
    ~ImageGenerationTask();
    void configure(const std::string& modelPath = "");
    // Génère une image au format binaire (PNG/JPEG) renvoyée comme bytes
    std::vector<uint8_t> run(const std::string& prompt);
    static bool saveToFile(const std::string& outPath, const std::vector<uint8_t>& bytes);
    void log(const std::string& message) const;
private:
    std::string modelPath_;
    bool configured_;
};
