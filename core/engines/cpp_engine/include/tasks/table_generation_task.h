#pragma once
#include <string>
#include <stdexcept>

/**
 * @class TableGenerationTask
 * @brief Génération de tables/tabulaires (CSV/TSV) à partir d'un prompt.
 */
class TableGenerationTask {
public:
    TableGenerationTask();
    ~TableGenerationTask();
    void configure(const std::string& format = "csv");
    std::string run(const std::string& prompt);
    void log(const std::string& message) const;
private:
    std::string format_;
    bool configured_;
};
