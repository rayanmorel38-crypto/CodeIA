#ifndef CPP_ENGINE_FILTERS_IMAGE_FILTER_H
#define CPP_ENGINE_FILTERS_IMAGE_FILTER_H

#include <string>
#include <vector>
#include <cstdint>

namespace cppengine {
namespace filters {

/**
 * ImageFilter - Image processing and filtering
 * Blur, sharpen, color manipulation, edge detection
 */
class ImageFilter {
public:
    ImageFilter();
    ~ImageFilter();
    
    // Basic filters
    bool apply_blur(const std::string& input_file, const std::string& output_file, int radius);
    bool apply_sharpen(const std::string& input_file, const std::string& output_file, float strength);
    bool apply_gaussian_blur(const std::string& input_file, const std::string& output_file, int kernel_size);
    
    // Color operations
    bool adjust_brightness(const std::string& input_file, const std::string& output_file, float factor);
    bool adjust_contrast(const std::string& input_file, const std::string& output_file, float factor);
    bool adjust_saturation(const std::string& input_file, const std::string& output_file, float factor);
    
    // Edge detection
    bool detect_edges(const std::string& input_file, const std::string& output_file);
    
    // Morphological operations
    bool dilate(const std::string& input_file, const std::string& output_file, int kernel_size);
    bool erode(const std::string& input_file, const std::string& output_file, int kernel_size);
    
private:
    int thread_count_;
};

} // namespace filters
} // namespace cppengine

#endif // CPP_ENGINE_FILTERS_IMAGE_FILTER_H
