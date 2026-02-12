#pragma once
/**
 * @class TensorInferenceTask
 * @brief Inference rapide via ONNX Runtime ou backend similaire.
 *
 * Compilation optionnelle: définir `USE_ONNXRUNTIME` pour inclure l'API réelle.
 */
#include <string>
#include <vector>
#include <stdexcept>

class TensorInferenceTask {
public:
    TensorInferenceTask();
    ~TensorInferenceTask();
    void configure(const std::string& modelPath);
    std::vector<float> run(const std::vector<float>& input);
    void log(const std::string& message) const;
private:
    std::string modelPath_;
    bool configured_;
};
