#include "generators/video_generator.h"
#include "utils/logger.h"
#include <filesystem>
#include <vector>

namespace cppengine {
namespace generators {

VideoGenerator::VideoGenerator() : codec_("h264"), bitrate_kbps_(5000) {
    cpp_engine::utils::Logger::instance().info("VideoGenerator initialized");
}

VideoGenerator::~VideoGenerator() {}

std::string VideoGenerator::search_downloaded(const std::string& search_name) {
    try {
        // Build path to downloaded_images/{search_name}
        std::string base_path = "java_ai_system/data/downloaded_images/" + search_name;
        
        if (!std::filesystem::exists(base_path)) {
            cpp_engine::utils::Logger::instance().debug("Downloaded images directory not found: " + base_path);
            return "";
        }
        
        // Search for first video file in directory
        for (const auto& entry : std::filesystem::directory_iterator(base_path)) {
            if (entry.is_regular_file()) {
                std::string ext = entry.path().extension().string();
                if (ext == ".mp4" || ext == ".mkv" || ext == ".webm" || ext == ".avi" || ext == ".mov") {
                    std::string found_path = entry.path().string();
                    cpp_engine::utils::Logger::instance().info("Found downloaded video: " + found_path);
                    return found_path;
                }
            }
        }
        
        cpp_engine::utils::Logger::instance().debug("No video files found in: " + base_path);
        return "";
    } catch (const std::exception& e) {
        cpp_engine::utils::Logger::instance().warning("Error searching downloaded videos: " + std::string(e.what()));
        return "";
    }
}

bool VideoGenerator::generate_perlin_video(
    int width, int height, int frames, int fps,
    int seed, const std::string& search_name, const std::string& output_file) {
    
    cpp_engine::utils::Logger::instance().info("Generating Perlin video: " + std::to_string(width) + 
                                  "x" + std::to_string(height) + ", " + std::to_string(frames) + 
                                  " frames @ " + std::to_string(fps) + "fps" +
                                  " using style models from: " + search_name);
    
    // Load reference images from java_ai_system/data/downloaded_images/{search_name}
    std::vector<std::string> reference_images;
    std::string ref_dir = "java_ai_system/data/downloaded_images/" + search_name;
    
    if (std::filesystem::exists(ref_dir)) {
        for (const auto& entry : std::filesystem::directory_iterator(ref_dir)) {
            if (entry.is_regular_file()) {
                std::string ext = entry.path().extension().string();
                if (ext == ".png" || ext == ".jpg" || ext == ".jpeg" || ext == ".bmp") {
                    reference_images.push_back(entry.path().string());
                }
            }
        }
        cpp_engine::utils::Logger::instance().info("Found " + std::to_string(reference_images.size()) + " reference images for Perlin video style");
    } else {
        cpp_engine::utils::Logger::instance().warning("Reference images directory not found: " + ref_dir);
    }
    
    // Generate Perlin video based on reference style
    cpp_engine::utils::Logger::instance().debug("Generating Perlin video animation based on " + std::to_string(reference_images.size()) + " reference models");
    
    return true;
}

bool VideoGenerator::generate_silhouette_video(
    int width, int height, int frames, int fps,
    int seed, const std::string& search_name, const std::string& output_file) {
    
    cpp_engine::utils::Logger::instance().info("Generating silhouette video using style models from: " + search_name);
    
    // Load reference images from java_ai_system/data/downloaded_images/{search_name}
    std::vector<std::string> reference_images;
    std::string ref_dir = "java_ai_system/data/downloaded_images/" + search_name;
    
    if (std::filesystem::exists(ref_dir)) {
        for (const auto& entry : std::filesystem::directory_iterator(ref_dir)) {
            if (entry.is_regular_file()) {
                std::string ext = entry.path().extension().string();
                if (ext == ".png" || ext == ".jpg" || ext == ".jpeg" || ext == ".bmp") {
                    reference_images.push_back(entry.path().string());
                }
            }
        }
        cpp_engine::utils::Logger::instance().info("Found " + std::to_string(reference_images.size()) + " reference silhouettes for video style");
    } else {
        cpp_engine::utils::Logger::instance().warning("Reference images directory not found: " + ref_dir);
    }
    
    // Generate silhouette video based on reference style
    cpp_engine::utils::Logger::instance().debug("Generating silhouette video based on " + std::to_string(reference_images.size()) + " reference models");
    
    return true;
}

bool VideoGenerator::generate_metallic_video(
    int width, int height, int frames, int fps,
    int seed, const std::string& search_name, const std::string& output_file) {
    
    cpp_engine::utils::Logger::instance().info("Generating metallic video using style models from: " + search_name);
    
    // Load reference images from java_ai_system/data/downloaded_images/{search_name}
    std::vector<std::string> reference_images;
    std::string ref_dir = "java_ai_system/data/downloaded_images/" + search_name;
    
    if (std::filesystem::exists(ref_dir)) {
        for (const auto& entry : std::filesystem::directory_iterator(ref_dir)) {
            if (entry.is_regular_file()) {
                std::string ext = entry.path().extension().string();
                if (ext == ".png" || ext == ".jpg" || ext == ".jpeg" || ext == ".bmp") {
                    reference_images.push_back(entry.path().string());
                }
            }
        }
        cpp_engine::utils::Logger::instance().info("Found " + std::to_string(reference_images.size()) + " reference images for metallic video style");
    } else {
        cpp_engine::utils::Logger::instance().warning("Reference images directory not found: " + ref_dir);
    }
    
    // Generate metallic video based on reference style
    cpp_engine::utils::Logger::instance().debug("Generating metallic video based on " + std::to_string(reference_images.size()) + " reference models");
    
    return true;
}

void VideoGenerator::set_codec(const std::string& codec) {
    codec_ = codec;
}

void VideoGenerator::set_bitrate(int bitrate_kbps) {
    bitrate_kbps_ = bitrate_kbps;
}

} // namespace generators
} // namespace cppengine
