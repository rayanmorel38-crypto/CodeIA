#ifndef CPP_ENGINE_MODELS_TORCH_BACKEND_H
#define CPP_ENGINE_MODELS_TORCH_BACKEND_H

#include "models/model_backend.h"
#include <string>

#ifdef WITH_LIBTORCH
#include <torch/script.h>
#endif

namespace cppengine {
namespace models {

class TorchBackend : public ModelBackend {
public:
    TorchBackend();
    ~TorchBackend() override;

    bool load(const std::string& path) override;
    bool save(const std::string& path) override;

    bool train(const std::vector<std::string>& dataset_paths, const std::map<std::string,std::string>& params) override;
    bool incremental_learn(const std::string& sample_json) override;

    std::string infer(const std::string& input_json) override;
    std::string info() const override;

private:
    std::string model_path_;
    bool initialized_;
#ifdef WITH_LIBTORCH
    torch::jit::script::Module* module_ = nullptr;
#endif
};

} // namespace models
} // namespace cppengine

#endif // CPP_ENGINE_MODELS_TORCH_BACKEND_H
