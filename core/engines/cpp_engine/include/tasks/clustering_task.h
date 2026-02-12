#pragma once
/**
 * @class ClusteringTask
 * @brief Tâche de clustering (regroupement).
 */
#include <vector>
#include <string>
#include <stdexcept>

class ClusteringTask {
public:
    ClusteringTask();
    ~ClusteringTask();
    void configure(int nClusters);
    std::vector<int> run(const std::vector<std::vector<float>>& data);
    void log(const std::string& message) const;
private:
    int nClusters_;
    bool configured_;
};
