#ifndef CPP_ENGINE_SANDBOX_COMPARATOR_H
#define CPP_ENGINE_SANDBOX_COMPARATOR_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace cppengine {
namespace sandbox {

/**
 * Comparator for validating processing results
 * Compares actual outputs against expected outputs and known-good references
 */
class SandboxComparator {
public:
    /**
     * Comparison result structure
     */
    struct ComparisonResult {
        bool matches = false;
        double similarity = 0.0;  // 0.0 to 1.0
        std::string diff_report;
        std::vector<std::string> warnings;
        std::vector<std::string> errors;
        
        json to_json() const {
            json j;
            j["matches"] = matches;
            j["similarity"] = similarity;
            j["diff_report"] = diff_report;
            j["warnings"] = warnings;
            j["errors"] = errors;
            return j;
        }
    };

    /**
     * Validation mode
     */
    enum class ValidationMode {
        STRICT,      // Exact match required
        FUZZY,       // Fuzzy matching (allows small differences)
        REFERENCE,   // Compare against known-good reference
        BASELINE     // Compare against baseline metrics
    };

    SandboxComparator();
    ~SandboxComparator();
    
    /**
     * Set validation mode
     */
    void set_validation_mode(ValidationMode mode);
    
    /**
     * Register a reference output for comparison
     * @param task_name Name of the task
     * @param reference_output Expected output (as JSON or file path)
     */
    void register_reference(const std::string& task_name, const json& reference_output);
    
    /**
     * Compare actual output against reference
     * @param task_name Name of the task
     * @param actual_output Actual output from processing
     * @return Comparison result
     */
    ComparisonResult compare_output(const std::string& task_name, const json& actual_output);
    
    /**
     * Compare two images (pixel-by-pixel or perceptual)
     * @param expected_path Path to expected image
     * @param actual_path Path to actual image
     * @param perceptual Use perceptual comparison (more lenient)
     * @return Comparison result
     */
    ComparisonResult compare_images(const std::string& expected_path, 
                                   const std::string& actual_path,
                                   bool perceptual = false);
    
    /**
     * Compare two videos (frame sampling + metadata)
     * @param expected_path Path to expected video
     * @param actual_path Path to actual video
     * @param sample_frames Number of frames to sample
     * @return Comparison result
     */
    ComparisonResult compare_videos(const std::string& expected_path,
                                   const std::string& actual_path,
                                   int sample_frames = 10);
    
    /**
     * Validate filter output characteristics
     * Checks dimensions, format, metadata consistency
     */
    ComparisonResult validate_filter_output(const std::string& filter_type,
                                           const std::string& input_file,
                                           const std::string& output_file);
    
    /**
     * Validate effect output characteristics
     */
    ComparisonResult validate_effect_output(const std::string& effect_type,
                                           const std::string& input_file,
                                           const std::string& output_file);
    
    /**
     * Validate video processing output
     */
    ComparisonResult validate_video_output(const std::string& input_file,
                                          const std::string& output_file,
                                          const json& expected_config);
    
    /**
     * Get all validation errors
     */
    std::vector<std::string> get_errors() const;
    
    /**
     * Get all validation warnings
     */
    std::vector<std::string> get_warnings() const;
    
    /**
     * Reset validation state
     */
    void reset();
    
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
    
    // Helper methods
    double calculate_image_similarity(const std::string& img1, const std::string& img2);
    double calculate_video_similarity(const std::string& vid1, const std::string& vid2);
};

}  // namespace sandbox
}  // namespace cppengine

#endif // CPP_ENGINE_SANDBOX_COMPARATOR_H
