#include <opencv2/opencv.hpp>
#include <opencv2/features2d.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

namespace cpp_engine::modules::vision {

class FeatureExtractor {
public:
    static std::string name() { return "feature_extractor"; }

    // Extract ORB descriptors and save to a simple binary file
    static std::string extract_and_save(const std::string &image_path, const std::string &out_path) {
        cv::Mat img = cv::imread(image_path, cv::IMREAD_GRAYSCALE);
        if (img.empty()) throw std::runtime_error("Failed to read image: " + image_path);

        auto orb = cv::ORB::create(500);
        std::vector<cv::KeyPoint> kps;
        cv::Mat desc;
        orb->detectAndCompute(img, cv::noArray(), kps, desc);

        fs::create_directories(fs::path(out_path).parent_path());
        std::ofstream out(out_path, std::ios::binary);
        if (!out) throw std::runtime_error("Failed to open: " + out_path);

        int rows = desc.rows, cols = desc.cols;
        out.write(reinterpret_cast<const char*>(&rows), sizeof(rows));
        out.write(reinterpret_cast<const char*>(&cols), sizeof(cols));
        out.write(reinterpret_cast<const char*>(desc.data), desc.total() * desc.elemSize());
        out.close();

        return out_path;
    }
};

} // namespace cpp_engine::modules::vision
