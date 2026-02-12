#pragma once
#include <vector>
#include <string>
#include <stdexcept>

/**
 * @class SegmentationTask
 * @brief Segmentation d'image (renvoie masque binaire simulé).
 */
class SegmentationTask {
public:
    SegmentationTask();
    ~SegmentationTask();
    void configure(const std::string& model = "");
    std::vector<uint8_t> run(const std::string& imagePath);
    void log(const std::string& message) const;
private:
    std::string model_;
    bool configured_;
};
