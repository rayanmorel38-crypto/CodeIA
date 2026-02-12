#pragma once
/**
 * @class RecommendationTask
 * @brief Système de recommandation.
 */
#include <string>
#include <vector>
#include <stdexcept>

class RecommendationTask {
public:
    RecommendationTask();
    ~RecommendationTask();
    void configure(const std::vector<std::string>& items);
    std::vector<std::string> run(const std::string& userId);
    void log(const std::string& message) const;
private:
    std::vector<std::string> items_;
    bool configured_;
};
