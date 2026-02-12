#include "sandbox/comparator.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cmath>
#include <opencv2/opencv.hpp>

namespace cppengine {
namespace sandbox {

class SandboxComparator::Impl {
public:
    ValidationMode mode = ValidationMode::FUZZY;
    std::map<std::string, json> references;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
};

SandboxComparator::SandboxComparator() 
    : impl_(std::make_unique<Impl>()) {
}

SandboxComparator::~SandboxComparator() = default;

void SandboxComparator::set_validation_mode(ValidationMode mode) {
    impl_->mode = mode;
}

void SandboxComparator::register_reference(const std::string& task_name, 
                                          const json& reference_output) {
    impl_->references[task_name] = reference_output;
}

SandboxComparator::ComparisonResult SandboxComparator::compare_output(
    const std::string& task_name, 
    const json& actual_output) {
    
    ComparisonResult result;
    
    auto it = impl_->references.find(task_name);
    if (it == impl_->references.end()) {
        result.errors.push_back("No reference found for task: " + task_name);
        return result;
    }
    
    const auto& reference = it->second;
    
    if (impl_->mode == ValidationMode::STRICT) {
        // Exact JSON match
        result.matches = (reference == actual_output);
        result.similarity = result.matches ? 1.0 : 0.0;
        
        if (!result.matches) {
            result.diff_report = "Expected:\n" + reference.dump(2) + 
                               "\n\nGot:\n" + actual_output.dump(2);
        }
    } else if (impl_->mode == ValidationMode::FUZZY) {
        // Fuzzy matching - check key fields
        result.matches = true;
        double match_count = 0;
        double total_keys = 0;
        
        // Compare common keys
        for (auto& [key, value] : reference.items()) {
            total_keys += 1;
            if (actual_output.contains(key)) {
                if (actual_output[key] == value) {
                    match_count += 1;
                } else {
                    result.warnings.push_back(
                        "Field mismatch: " + key + 
                        " (expected: " + value.dump() + 
                        ", got: " + actual_output[key].dump() + ")"
                    );
                }
            } else {
                result.warnings.push_back("Missing field: " + key);
            }
        }
        
        result.similarity = (total_keys > 0) ? (match_count / total_keys) : 1.0;
        result.matches = (result.similarity >= 0.8);  // 80% threshold
    }
    
    return result;
}

SandboxComparator::ComparisonResult SandboxComparator::compare_images(
    const std::string& expected_path,
    const std::string& actual_path,
    bool perceptual) {
    
    ComparisonResult result;
    
    try {
        cv::Mat expected = cv::imread(expected_path);
        cv::Mat actual = cv::imread(actual_path);
        
        if (expected.empty()) {
            result.errors.push_back("Cannot read expected image: " + expected_path);
            return result;
        }
        if (actual.empty()) {
            result.errors.push_back("Cannot read actual image: " + actual_path);
            return result;
        }
        
        // Check dimensions
        if (expected.size() != actual.size()) {
            result.warnings.push_back(
                "Image size mismatch: expected " + 
                std::to_string(expected.cols) + "x" + std::to_string(expected.rows) +
                ", got " + std::to_string(actual.cols) + "x" + std::to_string(actual.rows)
            );
        }
        
        // Check channels
        if (expected.channels() != actual.channels()) {
            result.warnings.push_back(
                "Channel mismatch: expected " + std::to_string(expected.channels()) +
                " channels, got " + std::to_string(actual.channels())
            );
        }
        
        // Calculate similarity
        result.similarity = calculate_image_similarity(expected_path, actual_path);
        
        if (impl_->mode == ValidationMode::STRICT) {
            result.matches = (result.similarity >= 0.99);  // Nearly identical
        } else {
            result.matches = (result.similarity >= 0.85);  // Fuzzy threshold
        }
        
        result.diff_report = "Image similarity: " + std::to_string(result.similarity);
        
    } catch (const std::exception& e) {
        result.errors.push_back(std::string("Image comparison error: ") + e.what());
    }
    
    return result;
}

SandboxComparator::ComparisonResult SandboxComparator::compare_videos(
    const std::string& expected_path,
    const std::string& actual_path,
    int sample_frames) {
    
    ComparisonResult result;
    
    try {
        cv::VideoCapture expected_vid(expected_path);
        cv::VideoCapture actual_vid(actual_path);
        
        if (!expected_vid.isOpened()) {
            result.errors.push_back("Cannot open expected video: " + expected_path);
            return result;
        }
        if (!actual_vid.isOpened()) {
            result.errors.push_back("Cannot open actual video: " + actual_path);
            return result;
        }
        
        // Check metadata
        int expected_frames = static_cast<int>(expected_vid.get(cv::CAP_PROP_FRAME_COUNT));
        int actual_frames = static_cast<int>(actual_vid.get(cv::CAP_PROP_FRAME_COUNT));
        
        if (std::abs(expected_frames - actual_frames) > 5) {
            result.warnings.push_back(
                "Frame count mismatch: expected " + std::to_string(expected_frames) +
                ", got " + std::to_string(actual_frames)
            );
        }
        
        double expected_fps = expected_vid.get(cv::CAP_PROP_FPS);
        double actual_fps = actual_vid.get(cv::CAP_PROP_FPS);
        
        if (std::abs(expected_fps - actual_fps) > 0.1) {
            result.warnings.push_back(
                "FPS mismatch: expected " + std::to_string(expected_fps) +
                ", got " + std::to_string(actual_fps)
            );
        }
        
        // Sample frames for similarity check
        result.similarity = calculate_video_similarity(expected_path, actual_path);
        result.matches = (result.similarity >= 0.80);
        
        result.diff_report = "Video similarity: " + std::to_string(result.similarity);
        
    } catch (const std::exception& e) {
        result.errors.push_back(std::string("Video comparison error: ") + e.what());
    }
    
    return result;
}

SandboxComparator::ComparisonResult SandboxComparator::validate_filter_output(
    const std::string& filter_type,
    const std::string& input_file,
    const std::string& output_file) {
    
    ComparisonResult result;
    
    try {
        cv::Mat input = cv::imread(input_file);
        cv::Mat output = cv::imread(output_file);
        
        if (input.empty()) {
            result.errors.push_back("Cannot read input file: " + input_file);
            return result;
        }
        if (output.empty()) {
            result.errors.push_back("Cannot read output file: " + output_file);
            return result;
        }
        
        // Validate filter-specific characteristics
        result.matches = true;
        result.similarity = 1.0;
        
        if (filter_type == "grayscale") {
            if (output.channels() != 1) {
                result.errors.push_back("Grayscale filter should produce 1-channel image");
                result.matches = false;
            }
        } else if (filter_type == "blur" || filter_type == "sharpen") {
            // Dimensions should match
            if (input.size() != output.size()) {
                result.errors.push_back("Filter output dimensions don't match input");
                result.matches = false;
            }
            // Content should be similar but not identical
            result.similarity = calculate_image_similarity(input_file, output_file);
            if (result.similarity > 0.95) {
                result.warnings.push_back("Output too similar to input (possible processing failure)");
            }
        } else {
            result.warnings.push_back("Validation not implemented for filter: " + filter_type);
        }
        
    } catch (const std::exception& e) {
        result.errors.push_back(std::string("Filter validation error: ") + e.what());
    }
    
    return result;
}

SandboxComparator::ComparisonResult SandboxComparator::validate_effect_output(
    const std::string& effect_type,
    const std::string& input_file,
    const std::string& output_file) {
    
    ComparisonResult result;
    
    try {
        cv::Mat input = cv::imread(input_file);
        cv::Mat output = cv::imread(output_file);
        
        if (input.empty() || output.empty()) {
            result.errors.push_back("Cannot read input or output file");
            return result;
        }
        
        // Validate effect-specific characteristics
        result.matches = true;
        result.similarity = calculate_image_similarity(input_file, output_file);
        
        if (effect_type == "edge_detect") {
            // Should be significantly different from input
            if (result.similarity > 0.8) {
                result.errors.push_back("Edge detection output too similar to input");
                result.matches = false;
            }
        } else if (effect_type == "posterize") {
            // Dimensions should match
            if (input.size() != output.size()) {
                result.errors.push_back("Effect output dimensions don't match input");
                result.matches = false;
            }
        }
        
    } catch (const std::exception& e) {
        result.errors.push_back(std::string("Effect validation error: ") + e.what());
    }
    
    return result;
}

SandboxComparator::ComparisonResult SandboxComparator::validate_video_output(
    const std::string& input_file,
    const std::string& output_file,
    const json& expected_config) {
    
    ComparisonResult result;
    
    try {
        cv::VideoCapture vid(output_file);
        
        if (!vid.isOpened()) {
            result.errors.push_back("Cannot open output video: " + output_file);
            return result;
        }
        
        int width = static_cast<int>(vid.get(cv::CAP_PROP_FRAME_WIDTH));
        int height = static_cast<int>(vid.get(cv::CAP_PROP_FRAME_HEIGHT));
        double fps = vid.get(cv::CAP_PROP_FPS);
        
        // Check against expected config
        result.matches = true;
        
        if (expected_config.contains("width")) {
            if (width != expected_config["width"]) {
                result.warnings.push_back("Width mismatch");
            }
        }
        if (expected_config.contains("height")) {
            if (height != expected_config["height"]) {
                result.warnings.push_back("Height mismatch");
            }
        }
        if (expected_config.contains("fps")) {
            if (std::abs(fps - expected_config["fps"].get<double>()) > 0.5) {
                result.warnings.push_back("FPS mismatch");
            }
        }
        
        result.similarity = 0.9;  // Placeholder
        
    } catch (const std::exception& e) {
        result.errors.push_back(std::string("Video validation error: ") + e.what());
    }
    
    return result;
}

std::vector<std::string> SandboxComparator::get_errors() const {
    return impl_->errors;
}

std::vector<std::string> SandboxComparator::get_warnings() const {
    return impl_->warnings;
}

void SandboxComparator::reset() {
    impl_->errors.clear();
    impl_->warnings.clear();
}

double SandboxComparator::calculate_image_similarity(const std::string& img1,
                                                     const std::string& img2) {
    try {
        cv::Mat m1 = cv::imread(img1);
        cv::Mat m2 = cv::imread(img2);
        
        if (m1.empty() || m2.empty()) return 0.0;
        
        // Resize to same dimensions if needed
        if (m1.size() != m2.size()) {
            cv::resize(m2, m2, m1.size());
        }
        
        // Calculate MSE (Mean Squared Error)
        cv::Mat diff;
        cv::absdiff(m1, m2, diff);
        diff.convertTo(diff, CV_32F);
        diff = diff.mul(diff);
        
        double mse = cv::mean(diff)[0];
        
        // Convert MSE to similarity (0-1 range)
        // Normalized to typical image value range [0-255]
        double max_mse = 255.0 * 255.0;
        return 1.0 - std::min(1.0, mse / max_mse);
        
    } catch (...) {
        return 0.0;
    }
}

double SandboxComparator::calculate_video_similarity(const std::string& vid1,
                                                     const std::string& vid2) {
    try {
        cv::VideoCapture cap1(vid1);
        cv::VideoCapture cap2(vid2);
        
        if (!cap1.isOpened() || !cap2.isOpened()) return 0.0;
        
        int frames_to_check = 5;
        double total_similarity = 0.0;
        
        for (int i = 0; i < frames_to_check; ++i) {
            cv::Mat frame1, frame2;
            cap1 >> frame1;
            cap2 >> frame2;
            
            if (frame1.empty() || frame2.empty()) break;
            
            // Skip some frames for sampling
            for (int j = 0; j < 30; ++j) {
                cap1 >> frame1;
                cap2 >> frame2;
                if (frame1.empty() || frame2.empty()) break;
            }
            
            if (!frame1.empty() && !frame2.empty()) {
                total_similarity += calculate_image_similarity(vid1, vid2);
            }
        }
        
        return total_similarity / frames_to_check;
        
    } catch (...) {
        return 0.0;
    }
}

}  // namespace sandbox
}  // namespace cppengine
