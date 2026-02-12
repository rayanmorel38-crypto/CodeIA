#ifndef CPP_ENGINE_API_H
#define CPP_ENGINE_API_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace cppengine {
namespace api {

/**
 * Result structure for all operations
 */
struct Result {
    bool success = false;
    std::string message;
    json data;
    std::string error;
    
    json to_json() const {
        json j;
        j["success"] = success;
        j["message"] = message;
        j["error"] = error;
        j["data"] = data;
        return j;
    }
};

/**
 * Filter configuration
 */
struct FilterConfig {
    std::string filter_type;     // "blur", "sharpen", "grayscale", etc.
    std::string input_path;
    std::string output_path;
    json parameters;             // Additional filter parameters
};

/**
 * Effect configuration
 */
struct EffectConfig {
    std::string effect_type;     // "edge_detect", "posterize", etc.
    std::string input_path;
    std::string output_path;
    json parameters;             // Additional effect parameters
};

/**
 * Video processing configuration
 */
struct VideoConfig {
    std::string input_path;
    std::string output_path;
    std::string codec = "h264";
    int quality = 85;
    int fps = 30;
    json filters;               // Array of filter configs
    json effects;               // Array of effect configs
};

/**
 * Core C++ Engine API
 * Provides modular access to all processing functions
 */
class Engine {
public:
    Engine();
    ~Engine();
    
    /**
     * Initialize the engine with configuration
     */
    Result initialize(const json& config);
    
    /**
     * Apply a filter to an image
     */
    Result apply_filter(const FilterConfig& config);
    
    /**
     * Apply an effect to an image
     */
    Result apply_effect(const EffectConfig& config);
    
    /**
     * Process a video file
     */
    Result process_video(const VideoConfig& config);
    
    /**
     * Analyze an image
     */
    Result analyze_image(const std::string& image_path);
    
    /**
     * Generate video from parameters
     */
    Result generate_video(const json& config);
    
    /**
     * Get available filters
     */
    std::vector<std::string> get_available_filters() const;
    
    /**
     * Get available effects
     */
    std::vector<std::string> get_available_effects() const;
    
    /**
     * Get engine info
     */
    json get_info() const;
    
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

}  // namespace api
}  // namespace cppengine

#endif // CPP_ENGINE_API_H
