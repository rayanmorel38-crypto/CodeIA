#pragma once
/**
 * @class DecompressionTask
 * @brief Décompression correspondante à `CompressionTask`.
 */
#include <string>
#include <vector>
#include <stdexcept>

class DecompressionTask {
public:
    DecompressionTask();
    ~DecompressionTask();
    void configure();
    std::vector<uint8_t> run(const std::vector<uint8_t>& input);
    void log(const std::string& message) const;
private:
    bool configured_;
};
