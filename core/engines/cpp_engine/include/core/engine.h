#ifndef CPP_ENGINE_CORE_ENGINE_H
#define CPP_ENGINE_CORE_ENGINE_H

#include <string>
#include <vector>
#include <memory>
#include <map>

// Forward declare ModelManager (defined in models/model_manager.h)
namespace cppengine { namespace models { class ModelManager; } }

namespace cppengine {
namespace core {

/**
 * CPPEngine - Main orchestrator for image/video generation
 * Coordinates between generators, filters, effects, and optimization modules
 */
class CPPEngine {
public:
    CPPEngine();
    ~CPPEngine();
    
    // Health checks
    bool is_healthy() const;
    std::string get_version() const;
    std::string get_capabilities() const;
    
    // Task management
    void add_task(const std::string& task_id, const std::string& task_type);
    void execute_task(const std::string& task_id);
    void execute_all_tasks();
    
    // Configuration
    void configure(const std::map<std::string, std::string>& config);
    std::string get_config(const std::string& key) const;
    
    // Deep learning / self-learning capabilities
    bool train_model(const std::vector<std::string>& dataset_paths, const std::map<std::string, std::string>& params);
    bool incremental_learn(const std::string& sample_json);
    bool load_model(const std::string& model_path);
    std::string infer_model(const std::string& input_json);
    
    // Personal AI interaction
    std::string personal_interaction(const std::string& user_input);
    
    // New system modules
    void schedule_task(const std::string& task_id, int delay_seconds, bool recurring = false);
    std::string get_system_status();
    std::string network_request(const std::string& endpoint, const std::string& method, const std::string& payload = "");
    
private:
    struct Task {
        std::string id;
        std::string type;
        std::map<std::string, std::string> params;
    };
    
    std::vector<Task> tasks_;
    std::map<std::string, std::string> config_;
    // Model manager (deep learning)
    std::unique_ptr<class cppengine::models::ModelManager> model_manager_;
};

} // namespace core
} // namespace cppengine

#endif // CPP_ENGINE_CORE_ENGINE_H
