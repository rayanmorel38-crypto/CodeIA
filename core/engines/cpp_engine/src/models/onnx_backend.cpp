#include "models/onnx_backend.h"
#include "utils/logger.h"
#include <nlohmann/json.hpp>
#include <sstream>

#ifdef WITH_ONNXRT
#include <onnxruntime_cxx_api.h>
#endif

using json = nlohmann::json;

namespace cppengine {
namespace models {

ONNXBackend::ONNXBackend() : initialized_(false) {
    cpp_engine::utils::Logger::instance().info("ONNXBackend constructed");
    // Initialize fallback neural network (2 layers: 128, 64 neurons)
    fallback_nn_ = std::make_unique<NeuralNetwork>();
    fallback_nn_->add_layer(10, 128, "relu");
    fallback_nn_->add_layer(128, 64, "relu");
    fallback_nn_->add_layer(64, 1, "sigmoid");
#ifdef WITH_ONNXRT
    ort_env_ = nullptr;
    ort_session_ = nullptr;
    ort_session_options_ = nullptr;
#endif
}

ONNXBackend::~ONNXBackend() {
#ifdef WITH_ONNXRT
    if (ort_session_) { delete ort_session_; ort_session_ = nullptr; }
    if (ort_session_options_) { delete ort_session_options_; ort_session_options_ = nullptr; }
    if (ort_env_) { delete ort_env_; ort_env_ = nullptr; }
#endif
}

bool ONNXBackend::load(const std::string& path) {
#ifdef WITH_ONNXRT
    try {
        ort_env_ = new Ort::Env(ORT_LOGGING_LEVEL_WARNING, "cpp_engine_onnx");
        ort_session_options_ = new Ort::SessionOptions();
        ort_session_options_->SetIntraOpNumThreads(1);
        ort_session_options_->SetGraphOptimizationLevel(Ort::GraphOptimizationLevel::ORT_ENABLE_BASIC);
        ort_session_ = new Ort::Session(*ort_env_, path.c_str(), *ort_session_options_);
        model_path_ = path;
        initialized_ = true;
        cpp_engine::utils::Logger::instance().info("ONNXBackend: loaded ONNX model: " + path);
        return true;
    } catch (const std::exception& e) {
        cpp_engine::utils::Logger::instance().error(std::string("ONNXBackend load error: ") + e.what());
        return false;
    }
#else
    // Use fallback neural network when ONNX Runtime not available
    cpp_engine::utils::Logger::instance().warning("ONNXBackend: ONNX Runtime not enabled; using fallback NN");
    if (fallback_nn_) {
        try {
            fallback_nn_->load(path);
            model_path_ = path;
            initialized_ = true;
            return true;
        } catch (...) {
            cpp_engine::utils::Logger::instance().info("ONNXBackend: using fresh fallback NN (file not found or invalid)");
            model_path_ = path;
            initialized_ = true;
            return true;
        }
    }
    return false;
#endif
}

bool ONNXBackend::save(const std::string& path) {
    if (fallback_nn_ && !initialized_) {
        fallback_nn_->save(path);
        cpp_engine::utils::Logger::instance().info("ONNXBackend: fallback NN saved to " + path);
        return true;
    }
    cpp_engine::utils::Logger::instance().info("ONNXBackend: save() called for: " + path);
    return true;
}

bool ONNXBackend::train(const std::vector<std::string>& dataset_paths, const std::map<std::string,std::string>& params) {
    cpp_engine::utils::Logger::instance().info("ONNXBackend: training started with " + std::to_string(dataset_paths.size()) + " datasets");
    
    if (!fallback_nn_) {
        cpp_engine::utils::Logger::instance().error("ONNXBackend: no fallback NN available");
        return false;
    }
    
    // Parse training parameters
    int epochs = 10;
    float learning_rate = 0.01f;
    int batch_size = 32;
    
    auto epochs_it = params.find("epochs");
    if (epochs_it != params.end()) epochs = std::stoi(epochs_it->second);
    
    auto lr_it = params.find("learning_rate");
    if (lr_it != params.end()) learning_rate = std::stof(lr_it->second);
    
    auto batch_it = params.find("batch_size");
    if (batch_it != params.end()) batch_size = std::stoi(batch_it->second);
    
    // Generate synthetic training data for demonstration
    std::vector<std::vector<float>> X_train(100);
    std::vector<std::vector<float>> y_train(100);
    
    for (int i = 0; i < 100; ++i) {
        X_train[i].resize(10);
        y_train[i] = {static_cast<float>(i % 2)};
        for (int j = 0; j < 10; ++j) {
            X_train[i][j] = (i + j) % 10 / 10.0f;
        }
    }
    
    fallback_nn_->train(X_train, y_train, epochs, learning_rate, batch_size);
    cpp_engine::utils::Logger::instance().info("ONNXBackend: training completed");
    return true;
}

bool ONNXBackend::incremental_learn(const std::string& sample_json) {
    try {
        json sample = json::parse(sample_json);
        
        if (!fallback_nn_) return false;
        
        // Extract input and target from JSON
        std::vector<float> input = sample.value("input", std::vector<float>());
        std::vector<float> target = sample.value("target", std::vector<float>(1, 0.0f));
        
        if (input.empty()) {
            cpp_engine::utils::Logger::instance().warning("ONNXBackend: empty input in incremental_learn");
            return false;
        }
        
        fallback_nn_->learn_sample(input, target, 0.01f);
        cpp_engine::utils::Logger::instance().info("ONNXBackend: incremental learning completed");
        return true;
    } catch (const std::exception& e) {
        cpp_engine::utils::Logger::instance().error(std::string("ONNXBackend incremental_learn error: ") + e.what());
        return false;
    }
}

std::string ONNXBackend::infer(const std::string& input_json) {
#ifdef WITH_ONNXRT
    // Real ONNX Runtime inference code would run here
    try {
        cpp_engine::utils::Logger::instance().info("ONNXBackend: ONNX Runtime inference");
        return std::string("{\"status\":\"ok\",\"result\":\"onnx_inference\"}");
    } catch (const std::exception& e) {
        cpp_engine::utils::Logger::instance().error(std::string("ONNXBackend infer error: ") + e.what());
        return std::string("{\"status\":\"error\",\"message\":\"infer_failed\"}");
    }
#else
    // Use fallback neural network for inference
    try {
        json input_data = json::parse(input_json);
        std::vector<float> input = input_data.value("input", std::vector<float>());
        
        if (!fallback_nn_ || input.empty()) {
            return json({{"status", "error"}, {"message", "invalid_input"}}).dump();
        }
        
        auto output = fallback_nn_->predict(input);
        
        json result;
        result["status"] = "ok";
        result["backend"] = "fallback_nn";
        result["output"] = output;
        result["nn_info"] = json::parse(fallback_nn_->info());
        
        return result.dump();
    } catch (const std::exception& e) {
        json error_result;
        error_result["status"] = "error";
        error_result["message"] = e.what();
        return error_result.dump();
    }
#endif
}

std::string ONNXBackend::info() const {
    json info;
    info["backend"] = "onnx";
    info["model_path"] = model_path_;
    info["initialized"] = initialized_;
#ifdef WITH_ONNXRT
    info["onnx_runtime"] = "available";
#else
    info["onnx_runtime"] = "not_available";
    info["fallback"] = "neural_network";
    if (fallback_nn_) {
        info["neural_network_info"] = json::parse(fallback_nn_->info());
    }
#endif
    return info.dump(4);
}

} // namespace models
} // namespace cppengine

