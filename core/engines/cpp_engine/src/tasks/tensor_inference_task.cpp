#include "../../include/tasks/tensor_inference_task.h"
#include "utils/logger.h"

TensorInferenceTask::TensorInferenceTask() : configured_(false) {}
TensorInferenceTask::~TensorInferenceTask() {}

void TensorInferenceTask::configure(const std::string& modelPath) {
    modelPath_ = modelPath;
    configured_ = true;
    log("Model configured: " + modelPath_);
}

std::vector<float> TensorInferenceTask::run(const std::vector<float>& input) {
    if (!configured_) throw std::runtime_error("TensorInferenceTask non configurée");
    log("Lancement inference (placeholder)");
#ifdef USE_ONNXRUNTIME
    // Code réel ONNX Runtime ici
    log("ONNX Runtime path (placeholder)");
    return std::vector<float>(1, 0.0f);
#else
    // Simulation: renvoyer un vecteur de mêmes dimensions
    std::vector<float> out(input.size(), 0.0f);
    return out;
#endif
}

void TensorInferenceTask::log(const std::string& message) const {
    cpp_engine::utils::Logger::instance().info(std::string("[TensorInferenceTask] ") + message);
}
