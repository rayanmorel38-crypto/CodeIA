#pragma once
/**
 * @class GraphGenerationTask
 * @brief Génération de graphiques ou graphes.
 */
#include <string>
#include <stdexcept>

class GraphGenerationTask {
public:
    GraphGenerationTask();
    ~GraphGenerationTask();
    void configure(const std::string& type);
    bool run(const std::string& outputPath);
    void log(const std::string& message) const;
private:
    std::string type_;
    bool configured_;
};
