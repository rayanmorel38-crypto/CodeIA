#include "../../include/tasks/gpu_acceleration_task.h"
#include <iostream>

GPUAccelerationTask::GPUAccelerationTask() : configured_(false) {}
GPUAccelerationTask::~GPUAccelerationTask() {}

void GPUAccelerationTask::configure(const std::string& device) {
    device_ = device;
    configured_ = true;
    log("GPU device configured: " + device_);
}

bool GPUAccelerationTask::run(const std::string& workloadDescription) {
    if (!configured_) throw std::runtime_error("GPUAccelerationTask non configurée");
    log("Exécution workload GPU: " + workloadDescription);
#ifdef USE_CUDA
    // Implémentation CUDA réelle ici
    log("CUDA path: exécution GPU (placeholder)");
    return true;
#elif defined(USE_OPENCL)
    log("OpenCL path: exécution GPU (placeholder)");
    return true;
#else
    log("Aucun backend GPU activé. Exécution en mode simulé.");
    return false;
#endif
}

void GPUAccelerationTask::log(const std::string& message) const {
    std::cout << "[GPUAccelerationTask] " << message << std::endl;
}
