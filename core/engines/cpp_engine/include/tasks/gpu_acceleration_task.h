#pragma once
/**
 * @class GPUAccelerationTask
 * @brief Interface pour accélération GPU (CUDA/OpenCL). Compilation optionnelle.
 *
 * Définir `USE_CUDA` ou `USE_OPENCL` dans le CMake pour activer l'impl réelle.
 */
#include <string>
#include <stdexcept>

class GPUAccelerationTask {
public:
    GPUAccelerationTask();
    ~GPUAccelerationTask();
    void configure(const std::string& device);
    bool run(const std::string& workloadDescription);
    void log(const std::string& message) const;
private:
    std::string device_;
    bool configured_;
};
