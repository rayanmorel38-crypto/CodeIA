#ifndef CPP_ENGINE_GENERATORS_VIDEO_GENERATOR_H
#define CPP_ENGINE_GENERATORS_VIDEO_GENERATOR_H

#include <string>
#include <cstdint>

namespace cppengine {
namespace generators {

/**
 * VideoGenerator - High-performance video generation
 * Supports: Perlin-based animation, silhouette animation, metallic effects
 */
class VideoGenerator {
public:
    VideoGenerator();
    ~VideoGenerator();
    
    // Search for existing video in downloaded_images directory
    std::string search_downloaded(const std::string& search_name);
    
    // Video generation (search downloaded images first as style models, then generate video)
    bool generate_perlin_video(
        int width, int height, int frames, int fps,
        int seed, const std::string& search_name, const std::string& output_file
    );
    
    bool generate_silhouette_video(
        int width, int height, int frames, int fps,
        int seed, const std::string& search_name, const std::string& output_file
    );
    
    bool generate_metallic_video(
        int width, int height, int frames, int fps,
        int seed, const std::string& search_name, const std::string& output_file
    );
    
    // Configuration
    void set_codec(const std::string& codec);  // h264, h265, vp9
    void set_bitrate(int bitrate_kbps);
    
private:
    std::string codec_;
    int bitrate_kbps_;
};

} // namespace generators
} // namespace cppengine

#endif // CPP_ENGINE_GENERATORS_VIDEO_GENERATOR_H
