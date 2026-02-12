#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

namespace cpp_engine::modules::vision {

class Segmenter {
public:
    static std::string name() { return "segmenter"; }

    // Try GrabCut, fallback to simple Canny-based mask
    static std::string segment_to_png(const std::string &image_path, const std::string &out_path) {
        cv::Mat img = cv::imread(image_path);
        if (img.empty()) throw std::runtime_error("Failed to load image");

        cv::Mat mask(img.size(), CV_8UC1, cv::Scalar(cv::GC_PR_BGD));
        cv::Rect rect(img.cols/8, img.rows/8, img.cols*3/4, img.rows*3/4);
        cv::Mat bgdModel, fgdModel;
        try {
            cv::grabCut(img, mask, rect, bgdModel, fgdModel, 5, cv::GC_INIT_WITH_RECT);
            cv::Mat resultMask = (mask == cv::GC_FGD) | (mask == cv::GC_PR_FGD);
            cv::Mat out;
            img.copyTo(out, resultMask);

            fs::create_directories(fs::path(out_path).parent_path());
            cv::imwrite(out_path, out);
            return out_path;
        } catch (...) {
            // Fallback using Canny edges
            cv::Mat gray, edges;
            cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
            cv::Canny(gray, edges, 100, 200);
            cv::Mat color;
            cv::cvtColor(edges, color, cv::COLOR_GRAY2BGR);
            fs::create_directories(fs::path(out_path).parent_path());
            cv::imwrite(out_path, color);
            return out_path;
        }
    }
};

} // namespace cpp_engine::modules::vision
