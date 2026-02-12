#include "models/model_manager.h"
#include "models/onnx_backend.h"
#include "models/torch_backend.h"
#include "utils/logger.h"

namespace cppengine {
namespace models {

ModelManager::ModelManager(BackendType preferred) : active_backend_(BackendType::AUTO) {
    cpp_engine::utils::Logger::instance().info("ModelManager initialized");
    // Always initialize a default backend
    set_backend(preferred != BackendType::AUTO ? preferred : BackendType::AUTO);
}

ModelManager::~ModelManager() {}

bool ModelManager::set_backend(BackendType backend) {
    active_backend_ = backend;
    switch (backend) {
        case BackendType::ONNX:
            backend_impl_ = std::make_unique<ONNXBackend>();
            cpp_engine::utils::Logger::instance().info("ModelManager: backend set to ONNX");
            break;
        case BackendType::TORCH:
            backend_impl_ = std::make_unique<TorchBackend>();
            cpp_engine::utils::Logger::instance().info("ModelManager: backend set to TORCH");
            break;
        case BackendType::AUTO:
        default:
            // Prefer ONNX if available at runtime (the backend implementations will indicate availability)
            backend_impl_ = std::make_unique<ONNXBackend>();
            active_backend_ = BackendType::ONNX;
            cpp_engine::utils::Logger::instance().info("ModelManager: backend AUTO -> ONNX (default)");
            break;
    }
    return backend_impl_.get() != nullptr;
}

ModelManager::BackendType ModelManager::get_backend() const {
    return active_backend_;
}

bool ModelManager::load_model(const std::string& model_path) {
    if (!backend_impl_) {
        cpp_engine::utils::Logger::instance().error("ModelManager: no backend configured");
        return false;
    }
    current_model_path_ = model_path;
    return backend_impl_->load(model_path);
}

bool ModelManager::save_model(const std::string& model_path) {
    if (!backend_impl_) return false;
    return backend_impl_->save(model_path);
}

bool ModelManager::train_model(const std::vector<std::string>& dataset_paths, const std::map<std::string, std::string>& params) {
    if (!backend_impl_) return false;
    return backend_impl_->train(dataset_paths, params);
}

bool ModelManager::incremental_learn(const std::string& sample_json) {
    if (!backend_impl_) return false;
    return backend_impl_->incremental_learn(sample_json);
}

std::string ModelManager::infer(const std::string& input_json) {
    if (!backend_impl_) return std::string("{\"status\":\"error\",\"message\":\"no_backend\"}");
    return backend_impl_->infer(input_json);
}

std::string ModelManager::get_model_info() const {
    if (!backend_impl_) return std::string("no_backend");
    return backend_impl_->info();
}

} // namespace models
} // namespace cppengine
