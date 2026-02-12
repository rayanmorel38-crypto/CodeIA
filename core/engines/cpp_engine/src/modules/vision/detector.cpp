#include <opencv2/opencv.hpp>
#include <opencv2/objdetect.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>

namespace cpp_engine::modules::vision {

struct Detection { std::string label; float confidence; cv::Rect box; };

class Detector {
public:
    static std::string name() { return "detector"; }

    // Enhanced detector: bright blobs + face detection
    static std::string detect_json(const std::string &image_path) {
        cv::Mat img = cv::imread(image_path);
        if (img.empty()) {
            return "{\"error\": \"failed to read image\"}";
        }

        std::vector<Detection> detections;

        // Face detection using Haar cascades
        cv::CascadeClassifier face_cascade;
        if (face_cascade.load(cv::samples::findFile("haarcascade_frontalface_default.xml"))) {
            std::vector<cv::Rect> faces;
            cv::Mat gray;
            cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
            cv::equalizeHist(gray, gray);
            face_cascade.detectMultiScale(gray, faces, 1.1, 3, 0, cv::Size(30, 30));

            for (const auto& face : faces) {
                detections.push_back({"face", 0.9f, face});
            }
        }

        // Bright blob detection (fallback)
        cv::Mat gray;
        cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
        cv::Mat thresh;
        cv::threshold(gray, thresh, 200, 255, cv::THRESH_BINARY);

        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(thresh, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        for (auto &c : contours) {
            cv::Rect r = cv::boundingRect(c);
            if (r.area() < 100) continue;
            detections.push_back({"bright_blob", 0.7f, r});
        }

        // Serialize to JSON
        std::ostringstream ss;
        ss << "{\"detections\": [";
        bool first = true;
        for (const auto& det : detections) {
            if (!first) ss << ",";
            first = false;
            ss << "{\"label\": \"" << det.label << "\", \"confidence\": " << det.confidence
               << ", \"x\": " << det.box.x << ", \"y\": " << det.box.y
               << ", \"w\": " << det.box.width << ", \"h\": " << det.box.height << "}";
        }
        ss << "]}";
        return ss.str();
    }
};

} // namespace cpp_engine::modules::vision
