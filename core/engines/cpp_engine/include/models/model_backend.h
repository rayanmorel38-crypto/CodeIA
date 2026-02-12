#ifndef CPP_ENGINE_MODELS_MODEL_BACKEND_H
#define CPP_ENGINE_MODELS_MODEL_BACKEND_H

#include <string>
#include <vector>
#include <map>

namespace cppengine {
namespace models {

class ModelBackend {
public:
    virtual ~ModelBackend() = default;

    virtual bool load(const std::string& path) = 0;
    virtual bool save(const std::string& path) = 0;

    virtual bool train(const std::vector<std::string>& dataset_paths, const std::map<std::string,std::string>& params) = 0;
    virtual bool incremental_learn(const std::string& sample_json) = 0;

    virtual std::string infer(const std::string& input_json) = 0;
    virtual std::string info() const = 0;
};

} // namespace models
} // namespace cppengine

#endif // CPP_ENGINE_MODELS_MODEL_BACKEND_H
