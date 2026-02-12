// generators/image_generator.h
#pragma once

#include <string>
#include <vector>

namespace cpp_engine::generators {

class ImageGenerator {
public:
    ImageGenerator();
    ~ImageGenerator();
    
    void set_quality(int quality);
    bool generate_perlin(int width, int height, int seed, const std::string& output_path);
    bool generate_image(const std::string& prompt, const std::string& output_path, int width = 512, int height = 512);
    bool generate_image_from_text(const std::string& text, const std::string& output_path);
    std::vector<std::string> get_available_models();

private:
    int quality_;
};

} // namespace cpp_engine::generators