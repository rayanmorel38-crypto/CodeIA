#pragma once
/**
 * @class FeatureExtractionTask
 * @brief Extraction de caractéristiques à partir de données.
 */
#include <vector>
#include <string>
#include <stdexcept>

class FeatureExtractionTask {
public:
    FeatureExtractionTask();
    ~FeatureExtractionTask();
    void configure(const std::vector<std::string>& features);
    std::vector<float> run(const std::string& dataPath);
    void log(const std::string& message) const;
private:
    std::vector<std::string> features_;
    bool configured_;
};
