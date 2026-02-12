#ifndef CPP_ENGINE_MODELS_MODEL_MANAGER_H
#define CPP_ENGINE_MODELS_MODEL_MANAGER_H

#include <string>
#include <vector>
#include <map>
#include <memory>

#include "models/model_backend.h"

namespace cppengine {
namespace models {

/**
 * ModelManager
 * Gestionnaire multi-backend pour l'apprentissage et l'inférence.
 */
class ModelManager {
public:
    enum class BackendType { AUTO = 0, ONNX = 1, TORCH = 2 };

    explicit ModelManager(BackendType preferred = BackendType::AUTO);
    ~ModelManager();

    // Backend selection
    bool set_backend(BackendType backend);
    BackendType get_backend() const;

    // Deep learning lifecycle
    bool load_model(const std::string& model_path);
    bool save_model(const std::string& model_path);

    // Training (full training with dataset paths)
    bool train_model(const std::vector<std::string>& dataset_paths, const std::map<std::string, std::string>& params);

    // Incremental / online learning
    bool incremental_learn(const std::string& sample_json);

    // Inference API
    std::string infer(const std::string& input_json);

    // Model metadata
    std::string get_model_info() const;

private:
    std::unique_ptr<ModelBackend> backend_impl_;
    BackendType active_backend_;
    std::string current_model_path_;
};

} // namespace models
} // namespace cppengine

#endif // CPP_ENGINE_MODELS_MODEL_MANAGER_H
