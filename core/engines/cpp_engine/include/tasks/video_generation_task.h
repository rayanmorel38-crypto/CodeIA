#pragma once
#include <string>
#include <vector>
#include <stdexcept>

/**
 * @class VideoGenerationTask
 * @brief Génération de courtes vidéos à partir d'un prompt.
 */
class VideoGenerationTask {
public:
    VideoGenerationTask();
    ~VideoGenerationTask();
    void configure(const std::string& modelPath = "");
    // Retourne bytes vidéo (container simulé)
    std::vector<uint8_t> run(const std::string& prompt);
    static bool saveToFile(const std::string& outPath, const std::vector<uint8_t>& bytes);
    void log(const std::string& message) const;
private:
    std::string modelPath_;
    bool configured_;
};
