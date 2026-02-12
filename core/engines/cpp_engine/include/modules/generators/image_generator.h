#ifndef CPP_ENGINE_GENERATORS_IMAGE_GENERATOR_H
#define CPP_ENGINE_GENERATORS_IMAGE_GENERATOR_H

#include <string>
#include <vector>
#include <cstdint>

namespace cppengine {
namespace generators {

/**
 * ImageGenerator - High-performance image generation
 * Supports: Perlin noise, procedural synthesis, silhouette rendering
 */
class ImageGenerator {
public:
    ImageGenerator();
    ~ImageGenerator();
    
    // Search for existing image in downloaded_images directory
    std::string search_downloaded(const std::string& search_name);
    
    // Generation methods (search downloaded images first, then generate if needed)
    std::string generate_perlin(int width, int height, int seed, const std::string& search_name, const std::string& output_file);
    std::string generate_silhouette(int width, int height, int seed, const std::string& search_name, const std::string& output_file);
    std::string generate_metallic(int width, int height, int seed, const std::string& search_name, const std::string& output_file);
    
    // Batch generation
    std::vector<std::string> generate_batch(
        int count, int width, int height, 
        const std::string& generator_type, 
        int seed_base, 
        const std::string& output_dir,
        const std::string& search_name = ""
    );
    
    // Configuration
    void set_quality(int level);  // 1-9
    void set_color_palette(const std::vector<uint32_t>& colors);
    
private:
    int quality_level_;
    std::vector<uint32_t> color_palette_;
};

} // namespace generators
} // namespace cppengine

#endif // CPP_ENGINE_GENERATORS_IMAGE_GENERATOR_H
