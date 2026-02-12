#pragma once
/**
 * @class SearchTask
 * @brief Recherche intelligente dans des données.
 */
#include <string>
#include <vector>
#include <stdexcept>

class SearchTask {
public:
    SearchTask();
    ~SearchTask();
    void configure(const std::string& queryType);
    std::vector<std::string> run(const std::string& query);
    void log(const std::string& message) const;
private:
    std::string queryType_;
    bool configured_;
};
