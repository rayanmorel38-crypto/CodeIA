#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>
#include <array>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace fs = std::filesystem;

// Simple procedural 3D shape exporter (OBJ)
namespace cpp_engine { namespace modules { namespace generators {

class Image3DGenerator {
public:
    static std::string name() { return "image3d_generator"; }

    // Generate a simple low-poly sphere and write as OBJ
    static std::string generate_obj(const std::string &output_path, int lat = 8, int lon = 16, float radius = 1.0f) {
        fs::create_directories(fs::path(output_path).parent_path());
        std::ofstream out(output_path);
        if (!out) throw std::runtime_error("Failed to open " + output_path);

        std::vector<std::array<float,3>> verts;
        for (int i = 0; i <= lat; ++i) {
            float theta = static_cast<float>(i) / lat * M_PI;
            for (int j = 0; j < lon; ++j) {
                float phi = static_cast<float>(j) / lon * 2.0f * M_PI;
                float x = radius * std::sin(theta) * std::cos(phi);
                float y = radius * std::cos(theta);
                float z = radius * std::sin(theta) * std::sin(phi);
                verts.push_back(std::array<float,3>{x, y, z});
            }
        }

        for (auto &v : verts) out << "v " << v[0] << " " << v[1] << " " << v[2] << "\n";

        // Simple face generation
        int cols = lon;
        for (int i = 0; i < lat; ++i) {
            for (int j = 0; j < lon; ++j) {
                int a = i * cols + j + 1;
                int b = i * cols + ((j+1)%cols) + 1;
                int c = (i+1) * cols + ((j+1)%cols) + 1;
                int d = (i+1) * cols + j + 1;
                out << "f " << a << " " << b << " " << c << "\n";
                out << "f " << a << " " << c << " " << d << "\n";
            }
        }

        std::cout << "[Image3DGenerator] Wrote OBJ to " << output_path << std::endl;
        return output_path;
    }
};

} } }

extern "C" void register_module() {
    cpp_engine::modules::generators::Image3DGenerator::generate_obj("data/generated_3d/sphere.obj");
}
