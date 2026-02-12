#pragma once
#include <vector>
#include <string>
#include <stdexcept>

/**
 * @class ClassificationTask
 * @brief Classification simple d'entrées (simulée).
 */
class ClassificationTask {
public:
    ClassificationTask();
    ~ClassificationTask();
    void configure(const std::string& model = "");
    std::vector<int> run(const std::string& inputPath);
    void log(const std::string& message) const;
private:
    std::string model_;
    bool configured_;
};
