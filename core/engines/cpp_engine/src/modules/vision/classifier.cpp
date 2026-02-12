#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <map>

namespace cpp_engine::modules::vision {

class Classifier {
public:
    static std::string name() { return "classifier"; }

    // Very small color-histogram based classifier
    static std::pair<std::string, double> classify(const std::string &image_path) {
        cv::Mat img = cv::imread(image_path);
        if (img.empty()) return {"unknown", 0.0};

        cv::Mat hsv;
        cv::cvtColor(img, hsv, cv::COLOR_BGR2HSV);
        int h_bins = 30, s_bins = 32;
        int histSize[] = { h_bins, s_bins };
        float h_ranges[] = { 0, 180 };
        float s_ranges[] = { 0, 256 };
        const float* ranges[] = { h_ranges, s_ranges };
        int channels[] = { 0, 1 };

        cv::Mat hist;
        cv::calcHist(&hsv, 1, channels, cv::Mat(), hist, 2, histSize, ranges, true, false);
        cv::normalize(hist, hist, 0, 1, cv::NORM_MINMAX);

        // Simple heuristic: compute mean hue
        cv::Scalar mean_hsv = cv::mean(hsv);
        double hue = mean_hsv[0];
        if (hue < 30) return {"warm", 0.7};
        if (hue < 90) return {"greenish", 0.7};
        if (hue < 150) return {"cool", 0.7};
        return {"violetish", 0.6};
    }
};

} // namespace cpp_engine::modules::vision
