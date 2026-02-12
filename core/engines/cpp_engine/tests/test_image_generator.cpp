#include <iostream>
#include <cassert>
#include <filesystem>
#include "generators/image_generator.h"

namespace fs = std::filesystem;

void test_search_downloaded() {
    std::cout << "\n=== Test: search_downloaded() ===" << std::endl;
    
    cppengine::generators::ImageGenerator gen;
    
    // Test 1: Search for existing "robot_futuriste" images
    std::cout << "Test 1: Searching for 'robot_futuriste' images..." << std::endl;
    std::string found = gen.search_downloaded("robot_futuriste");
    if (!found.empty()) {
        std::cout << "✓ Found image: " << found << std::endl;
    } else {
        std::cout << "⚠ No images found in java_ai_system/data/downloaded_images/robot_futuriste/" << std::endl;
    }
    
    // Test 2: Search for non-existent directory
    std::cout << "\nTest 2: Searching for non-existent 'nonexistent' images..." << std::endl;
    std::string not_found = gen.search_downloaded("nonexistent");
    assert(not_found.empty());
    std::cout << "✓ Correctly returned empty string for missing directory" << std::endl;
}

void test_generate_perlin() {
    std::cout << "\n=== Test: generate_perlin() ===" << std::endl;
    
    cppengine::generators::ImageGenerator gen;
    
    // Check if reference images directory exists
    std::string ref_dir = "java_ai_system/data/downloaded_images/robot_futuriste";
    int ref_count = 0;
    
    if (fs::exists(ref_dir)) {
        for (const auto& entry : fs::directory_iterator(ref_dir)) {
            if (entry.is_regular_file()) {
                std::string ext = entry.path().extension().string();
                if (ext == ".png" || ext == ".jpg" || ext == ".jpeg" || ext == ".bmp") {
                    ref_count++;
                }
            }
        }
    }
    
    std::cout << "Found " << ref_count << " reference images in " << ref_dir << std::endl;
    
    // Test generate_perlin
    std::cout << "\nTest: Generating Perlin image with search_name='robot_futuriste'" << std::endl;
    bool result = gen.generate_perlin(512, 512, 42, "robot_futuriste", "data/generated_images/test_perlin.png");
    assert(result);
    std::cout << "✓ generate_perlin() returned success" << std::endl;
}

void test_generate_silhouette() {
    std::cout << "\n=== Test: generate_silhouette() ===" << std::endl;
    
    cppengine::generators::ImageGenerator gen;
    
    std::cout << "Test: Generating silhouette image with search_name='robot_futuriste'" << std::endl;
    bool result = gen.generate_silhouette(512, 512, 42, "robot_futuriste", "data/generated_images/test_silhouette.png");
    assert(result);
    std::cout << "✓ generate_silhouette() returned success" << std::endl;
}

void test_generate_metallic() {
    std::cout << "\n=== Test: generate_metallic() ===" << std::endl;
    
    cppengine::generators::ImageGenerator gen;
    
    std::cout << "Test: Generating metallic image with search_name='robot_futuriste'" << std::endl;
    bool result = gen.generate_metallic(512, 512, 42, "robot_futuriste", "data/generated_images/test_metallic.png");
    assert(result);
    std::cout << "✓ generate_metallic() returned success" << std::endl;
}

void test_quality_configuration() {
    std::cout << "\n=== Test: Configuration (quality level) ===" << std::endl;
    
    cppengine::generators::ImageGenerator gen;
    
    // Test valid quality levels
    std::cout << "Test: Setting quality to 5..." << std::endl;
    gen.set_quality(5);
    std::cout << "✓ Quality set to 5" << std::endl;
    
    // Test invalid quality level (should throw)
    std::cout << "\nTest: Setting invalid quality (10)..." << std::endl;
    try {
        gen.set_quality(10);
        std::cout << "✗ Should have thrown exception" << std::endl;
        assert(false);
    } catch (const std::exception& e) {
        std::cout << "✓ Correctly threw exception: " << e.what() << std::endl;
    }
}

int main() {
    std::cout << "=====================================" << std::endl;
    std::cout << "   ImageGenerator Unit Tests" << std::endl;
    std::cout << "=====================================" << std::endl;
    
    try {
        test_search_downloaded();
        test_generate_perlin();
        test_generate_silhouette();
        test_generate_metallic();
        test_quality_configuration();
        
        std::cout << "\n=====================================" << std::endl;
        std::cout << "✓ All tests passed!" << std::endl;
        std::cout << "=====================================" << std::endl;
        
        return 0;
    } catch (const std::exception& e) {
        std::cout << "\n✗ Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}
