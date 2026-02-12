#ifndef CPP_ENGINE_MODELS_ONNX_BACKEND_H
#define CPP_ENGINE_MODELS_ONNX_BACKEND_H

#include "models/model_backend.h"
#include "models/neural_network.h"
#include <string>
#include <memory>

#ifdef WITH_ONNXRT
#include <onnxruntime_cxx_api.h>
#endif

namespace cppengine {
namespace models {

class ONNXBackend : public ModelBackend {
public:
    ONNXBackend();
    ~ONNXBackend() override;

    bool load(const std::string& path) override;
    bool save(const std::string& path) override;

    bool train(const std::vector<std::string>& dataset_paths, const std::map<std::string,std::string>& params) override;
    bool incremental_learn(const std::string& sample_json) override;

    std::string infer(const std::string& input_json) override;
    std::string info() const override;

private:
    std::string model_path_;
    bool initialized_;
    std::unique_ptr<NeuralNetwork> fallback_nn_;  // Fallback when ONNX Runtime not available
#ifdef WITH_ONNXRT
    // ONNX Runtime C++ API objects (available when WITH_ONNXRT is enabled)
    Ort::Env* ort_env_ = nullptr;
    Ort::Session* ort_session_ = nullptr;
    Ort::SessionOptions* ort_session_options_ = nullptr;
#endif
};

} // namespace models
} // namespace cppengine

#endif // CPP_ENGINE_MODELS_ONNX_BACKEND_H
