// generators/image_generator.cpp
#include "generators/image_generator.h"
#include <iostream>

namespace cpp_engine::generators {

ImageGenerator::ImageGenerator() : quality_(8) {}

ImageGenerator::~ImageGenerator() {}

void ImageGenerator::set_quality(int quality) {
    quality_ = quality;
    std::cout << "ImageGenerator: quality set to " << quality_ << std::endl;
}

bool ImageGenerator::generate_perlin(int width, int height, int seed, const std::string& output_path) {
    (void)width; (void)height; (void)seed; (void)output_path;
    std::cout << "ImageGenerator: generate_perlin stub - " << width << "x" << height << ", seed: " << seed << std::endl;
    return true; // Stub implementation
}

bool ImageGenerator::generate_image(const std::string& prompt, const std::string& output_path, int width, int height) {
    (void)prompt; (void)output_path; (void)width; (void)height;
    std::cout << "ImageGenerator: generate_image stub - prompt: " << prompt << std::endl;
    return true; // Stub implementation
}

bool ImageGenerator::generate_image_from_text(const std::string& text, const std::string& output_path) {
    (void)text; (void)output_path;
    std::cout << "ImageGenerator: generate_image_from_text stub - text: " << text << std::endl;
    return true; // Stub implementation
}

std::vector<std::string> ImageGenerator::get_available_models() {
    return {"stub_model_v1", "stub_model_v2"};
}

} // namespace cpp_engine::generators