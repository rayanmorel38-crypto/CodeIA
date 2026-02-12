// generators/video_generator.h
#pragma once

#include <string>
#include <vector>

namespace cpp_engine::generators {

class VideoGenerator {
public:
    VideoGenerator();
    ~VideoGenerator();
    
    void set_codec(const std::string& codec);
    void set_bitrate(int bitrate);
    bool generate_perlin_video(int width, int height, int frames, int fps, int seed, const std::string& output_path);
    bool generate_video(const std::string& prompt, const std::string& output_path, int duration_seconds = 10);
    bool generate_video_from_images(const std::vector<std::string>& image_paths, const std::string& output_path);
    std::vector<std::string> get_available_models();

private:
    std::string codec_;
    int bitrate_;
};

} // namespace cpp_engine::generators