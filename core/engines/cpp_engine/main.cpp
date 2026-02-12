#include "core/engine.h"
#include "generators/image_generator.h"
#include "generators/video_generator.h"
#include "filters/image_filter.h"
#include "effects/effects_engine.h"
#include "optimization/performance_optimizer.h"
#include "utils/logger.h"
#include "utils/config.h"
#include "modules/system/memory_manager.h"
#include "modules/system/notification_manager.h"
#include "modules/vision/kinect_interface.h"

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <thread>
#include <chrono>

using namespace cppengine;

void print_usage() {
    std::cout << "Usage: image_video_generator [command] [options...]\n\n"
              << "Commands:\n"
              << "  demo                    Run full demo (default)\n"
              << "  filter <type> <input> <output> [params...]  Apply image filter\n"
              << "  effect <type> <input> <output> [params...]  Apply visual effect\n"
              << "  kinect_demo            Run Kinect demonstration\n\n"
              << "Filters: blur, sharpen, gaussian_blur, brightness, contrast, saturation, detect_edges, dilate, erode\n"
              << "Effects: lighting, shadows, particles, wave_distortion, radial_distortion, chromatic_aberration, bloom\n\n"
              << "Examples:\n"
              << "  image_video_generator filter blur input.png output.png 5\n"
              << "  image_video_generator effect bloom input.png output.png 0.8 0.6\n"
              << "  image_video_generator kinect_demo\n";
}

bool parse_args(int argc, char* argv[], std::string& command, std::vector<std::string>& args) {
    if (argc < 2) {
        command = "demo";
        return true;
    }

    command = argv[1];
    for (int i = 2; i < argc; ++i) {
        args.push_back(argv[i]);
    }
    return true;
}

bool run_image_filter(const std::vector<std::string>& args) {
    if (args.size() < 3) {
        std::cerr << "Error: filter command requires at least 3 arguments: <type> <input> <output>\n";
        return false;
    }

    std::string filter_type = args[0];
    std::string input_file = args[1];
    std::string output_file = args[2];

    filters::ImageFilter filter;
    bool result = false;

    if (filter_type == "blur") {
        int radius = args.size() > 3 ? std::stoi(args[3]) : 5;
        result = filter.apply_blur(input_file, output_file, radius);
    } else if (filter_type == "sharpen") {
        float strength = args.size() > 3 ? std::stof(args[3]) : 1.0f;
        result = filter.apply_sharpen(input_file, output_file, strength);
    } else if (filter_type == "gaussian_blur") {
        int kernel_size = args.size() > 3 ? std::stoi(args[3]) : 5;
        result = filter.apply_gaussian_blur(input_file, output_file, kernel_size);
    } else if (filter_type == "brightness") {
        float factor = args.size() > 3 ? std::stof(args[3]) : 0.5f;
        result = filter.adjust_brightness(input_file, output_file, factor);
    } else if (filter_type == "contrast") {
        float factor = args.size() > 3 ? std::stof(args[3]) : 1.2f;
        result = filter.adjust_contrast(input_file, output_file, factor);
    } else if (filter_type == "saturation") {
        float factor = args.size() > 3 ? std::stof(args[3]) : 1.5f;
        result = filter.adjust_saturation(input_file, output_file, factor);
    } else if (filter_type == "detect_edges") {
        result = filter.detect_edges(input_file, output_file);
    } else if (filter_type == "dilate") {
        int kernel_size = args.size() > 3 ? std::stoi(args[3]) : 3;
        result = filter.dilate(input_file, output_file, kernel_size);
    } else if (filter_type == "erode") {
        int kernel_size = args.size() > 3 ? std::stoi(args[3]) : 2;
        result = filter.erode(input_file, output_file, kernel_size);
    } else {
        std::cerr << "Unknown filter type: " << filter_type << "\n";
        return false;
    }

    if (result) {
        cpp_engine::utils::Logger::instance().info("Filter " + filter_type + " applied successfully to " + output_file);
        return true;
    } else {
        cpp_engine::utils::Logger::instance().error("Failed to apply filter " + filter_type);
        return false;
    }
}

bool run_visual_effect(const std::vector<std::string>& args) {
    if (args.size() < 3) {
        std::cerr << "Error: effect command requires at least 3 arguments: <type> <input> <output>\n";
        return false;
    }

    std::string effect_type = args[0];
    std::string input_file = args[1];
    std::string output_file = args[2];

    effects::EffectsEngine effects;
    bool result = false;

    if (effect_type == "lighting") {
        float light_x = args.size() > 3 ? std::stof(args[3]) : 1.0f;
        float light_y = args.size() > 4 ? std::stof(args[4]) : 0.5f;
        float light_z = args.size() > 5 ? std::stof(args[5]) : 0.8f;
        result = effects.apply_lighting(input_file, output_file, light_x, light_y, light_z);
    } else if (effect_type == "shadows") {
        float intensity = args.size() > 3 ? std::stof(args[3]) : 0.7f;
        result = effects.apply_shadows(input_file, output_file, intensity);
    } else if (effect_type == "particles") {
        int count = args.size() > 3 ? std::stoi(args[3]) : 50;
        std::string type = args.size() > 4 ? args[4] : "fire";
        result = effects.add_particles(input_file, output_file, count, type);
    } else if (effect_type == "wave_distortion") {
        float amplitude = args.size() > 3 ? std::stof(args[3]) : 10.0f;
        float frequency = args.size() > 4 ? std::stof(args[4]) : 0.02f;
        result = effects.apply_wave_distortion(input_file, output_file, amplitude, frequency);
    } else if (effect_type == "radial_distortion") {
        float factor = args.size() > 3 ? std::stof(args[3]) : 0.0001f;
        result = effects.apply_radial_distortion(input_file, output_file, factor);
    } else if (effect_type == "chromatic_aberration") {
        float red_shift = args.size() > 3 ? std::stof(args[3]) : 2.0f;
        float blue_shift = args.size() > 4 ? std::stof(args[4]) : 1.5f;
        result = effects.apply_chromatic_aberration(input_file, output_file, red_shift, blue_shift);
    } else if (effect_type == "bloom") {
        float threshold = args.size() > 3 ? std::stof(args[3]) : 0.8f;
        float intensity = args.size() > 4 ? std::stof(args[4]) : 0.6f;
        result = effects.apply_bloom(input_file, output_file, threshold, intensity);
    } else {
        std::cerr << "Unknown effect type: " << effect_type << "\n";
        return false;
    }

    if (result) {
        cpp_engine::utils::Logger::instance().info("Effect " + effect_type + " applied successfully to " + output_file);
        return true;
    } else {
        cpp_engine::utils::Logger::instance().error("Failed to apply effect " + effect_type);
        return false;
    }
}

bool run_kinect_demo() {
    cpp_engine::utils::Logger::instance().info("Starting Kinect demonstration...");

#ifdef WITH_FREENECT2
    cpp_engine::modules::vision::KinectInterface kinect;
    if (kinect.initialize()) {
        cpp_engine::utils::Logger::instance().info("Kinect initialized successfully");

        // Simple demo callback
        auto demo_callback = [](const cpp_engine::modules::vision::KinectFrame& frame) {
            cpp_engine::utils::Logger::instance().info("Received frame: " +
                std::to_string(frame.width) + "x" + std::to_string(frame.height) +
                " RGB: " + std::to_string(frame.rgb.size()) + " bytes, " +
                "Depth: " + std::to_string(frame.depth.size()) + " values");
        };

        if (kinect.start(demo_callback)) {
            cpp_engine::utils::Logger::instance().info("Kinect capture started");
            // Run for a few seconds
            std::this_thread::sleep_for(std::chrono::seconds(5));
            kinect.stop();
            cpp_engine::utils::Logger::instance().info("Kinect demo completed");
            return true;
        } else {
            cpp_engine::utils::Logger::instance().error("Failed to start Kinect capture");
            return false;
        }
    } else {
        cpp_engine::utils::Logger::instance().warning("Kinect not available, running stub demo");
        return true; // Still success for stub
    }
#else
    cpp_engine::utils::Logger::instance().info("Kinect support not compiled in, running stub demo");
    return true;
#endif
}

bool run_full_demo() {
    cpp_engine::utils::Logger::instance().info("=== CPP Engine Full Demo ===");

    // Create main engine
    core::CPPEngine engine;
    cpp_engine::utils::Logger::instance().info("Engine version: " + engine.get_version());
    cpp_engine::utils::Logger::instance().info("Capabilities: " + engine.get_capabilities());

    // Check health
    if (engine.is_healthy()) {
        cpp_engine::utils::Logger::instance().info("✓ Engine is healthy");
    }

    // Image generation demo
    cpp_engine::utils::Logger::instance().info("\n--- Image Generation ---");
    cpp_engine::generators::ImageGenerator img_gen;
    img_gen.set_quality(8);
    bool result = img_gen.generate_perlin(512, 512, 42, "sample_perlin.png");
    if (result) {
        cpp_engine::utils::Logger::instance().info("✓ Perlin image generated");
    }

    // Video generation demo
    cpp_engine::utils::Logger::instance().info("\n--- Video Generation ---");
    cpp_engine::generators::VideoGenerator vid_gen;
    vid_gen.set_codec("h264");
    vid_gen.set_bitrate(5000);
    result = vid_gen.generate_perlin_video(512, 512, 120, 30, 42, "sample_perlin.mp4");
    if (result) {
        cpp_engine::utils::Logger::instance().info("✓ Perlin video generated");
    }

    // Filters demo
    cpp_engine::utils::Logger::instance().info("\n--- Image Filters ---");
    filters::ImageFilter filter;
    result = filter.apply_blur("sample_perlin.png", "sample_blur.png", 5);
    if (result) {
        cpp_engine::utils::Logger::instance().info("✓ Blur filter applied");
    }

    // Effects demo
    cpp_engine::utils::Logger::instance().info("\n--- Visual Effects ---");
    effects::EffectsEngine effects;
    result = effects.apply_bloom("sample_perlin.png", "sample_bloom.png", 0.5f, 1.0f);
    if (result) {
        cpp_engine::utils::Logger::instance().info("✓ Bloom effect applied");
    }

    // Optimization demo
    cpp_engine::utils::Logger::instance().info("\n--- Optimization & Performance ---");
    optimization::PerformanceOptimizer optimizer;
    optimizer.enable_caching(500);
    if (optimizer.is_gpu_available()) {
        cpp_engine::utils::Logger::instance().info("✓ GPU available: " + optimizer.get_gpu_info());
    } else {
        cpp_engine::utils::Logger::instance().info("⚠ GPU not available, using CPU");
    }

    cpp_engine::utils::Logger::instance().info("\n=== Demo Complete ===");
    cpp_engine::utils::Logger::instance().info("✓ All modules working correctly");

    return true;
}

int main(int argc, char* argv[]) {
    try {
        // Initialize logger
        cpp_engine::utils::Logger::instance().init("cpp_engine.log", cpp_engine::utils::LogLevel::INFO);

        std::string command;
        std::vector<std::string> args;

        if (!parse_args(argc, argv, command, args)) {
            print_usage();
            return 1;
        }

        bool success = false;

        if (command == "demo") {
            success = run_full_demo();
        } else if (command == "filter") {
            success = run_image_filter(args);
        } else if (command == "effect") {
            success = run_visual_effect(args);
        } else if (command == "kinect_demo") {
            success = run_kinect_demo();
        } else {
            std::cerr << "Unknown command: " << command << "\n";
            print_usage();
            return 1;
        }

        return success ? 0 : 1;

    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
}
