#include "filters/image_filter.h"
#include "utils/logger.h"
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

namespace cppengine {
namespace filters {

ImageFilter::ImageFilter() : thread_count_(4) {
    cpp_engine::utils::Logger::instance().info("ImageFilter initialized with OpenCV");
}

ImageFilter::~ImageFilter() {}

bool ImageFilter::apply_blur(const std::string& input_file, const std::string& output_file, int radius) {
    try {
        cpp_engine::utils::Logger::instance().info("Applying blur filter, radius=" + std::to_string(radius));

        cv::Mat image = cv::imread(input_file);
        if (image.empty()) {
            cpp_engine::utils::Logger::instance().error("Failed to load image: " + input_file);
            return false;
        }

        cv::Mat blurred;
        cv::blur(image, blurred, cv::Size(radius, radius));

        if (cv::imwrite(output_file, blurred)) {
            cpp_engine::utils::Logger::instance().info("Blur filter applied successfully");
            return true;
        } else {
            cpp_engine::utils::Logger::instance().error("Failed to save blurred image: " + output_file);
            return false;
        }
    } catch (const cv::Exception& e) {
        cpp_engine::utils::Logger::instance().error("OpenCV error in blur: " + std::string(e.what()));
        return false;
    }
}

bool ImageFilter::apply_sharpen(const std::string& input_file, const std::string& output_file, float strength) {
    try {
        cpp_engine::utils::Logger::instance().info("Applying sharpen filter, strength=" + std::to_string(strength));

        cv::Mat image = cv::imread(input_file);
        if (image.empty()) {
            cpp_engine::utils::Logger::instance().error("Failed to load image: " + input_file);
            return false;
        }

        cv::Mat sharpened;
        cv::Mat kernel = (cv::Mat_<float>(3,3) << 0, -strength, 0,
                                                -strength, 1+4*strength, -strength,
                                                0, -strength, 0);
        cv::filter2D(image, sharpened, image.depth(), kernel);

        if (cv::imwrite(output_file, sharpened)) {
            cpp_engine::utils::Logger::instance().info("Sharpen filter applied successfully");
            return true;
        } else {
            cpp_engine::utils::Logger::instance().error("Failed to save sharpened image: " + output_file);
            return false;
        }
    } catch (const cv::Exception& e) {
        cpp_engine::utils::Logger::instance().error("OpenCV error in sharpen: " + std::string(e.what()));
        return false;
    }
}

bool ImageFilter::apply_gaussian_blur(const std::string& input_file, const std::string& output_file, int kernel_size) {
    try {
        cpp_engine::utils::Logger::instance().info("Applying Gaussian blur, kernel=" + std::to_string(kernel_size));

        cv::Mat image = cv::imread(input_file);
        if (image.empty()) {
            cpp_engine::utils::Logger::instance().error("Failed to load image: " + input_file);
            return false;
        }

        cv::Mat gaussian_blurred;
        cv::GaussianBlur(image, gaussian_blurred, cv::Size(kernel_size, kernel_size), 0);

        if (cv::imwrite(output_file, gaussian_blurred)) {
            cpp_engine::utils::Logger::instance().info("Gaussian blur applied successfully");
            return true;
        } else {
            cpp_engine::utils::Logger::instance().error("Failed to save Gaussian blurred image: " + output_file);
            return false;
        }
    } catch (const cv::Exception& e) {
        cpp_engine::utils::Logger::instance().error("OpenCV error in Gaussian blur: " + std::string(e.what()));
        return false;
    }
}

bool ImageFilter::adjust_brightness(const std::string& input_file, const std::string& output_file, float factor) {
    try {
        cpp_engine::utils::Logger::instance().info("Adjusting brightness, factor=" + std::to_string(factor));

        cv::Mat image = cv::imread(input_file);
        if (image.empty()) {
            cpp_engine::utils::Logger::instance().error("Failed to load image: " + input_file);
            return false;
        }

        cv::Mat brightness_adjusted;
        image.convertTo(brightness_adjusted, -1, 1.0, factor * 50); // factor * 50 pour un effet visible

        if (cv::imwrite(output_file, brightness_adjusted)) {
            cpp_engine::utils::Logger::instance().info("Brightness adjusted successfully");
            return true;
        } else {
            cpp_engine::utils::Logger::instance().error("Failed to save brightness adjusted image: " + output_file);
            return false;
        }
    } catch (const cv::Exception& e) {
        cpp_engine::utils::Logger::instance().error("OpenCV error in brightness: " + std::string(e.what()));
        return false;
    }
}

bool ImageFilter::adjust_contrast(const std::string& input_file, const std::string& output_file, float factor) {
    try {
        cpp_engine::utils::Logger::instance().info("Adjusting contrast, factor=" + std::to_string(factor));

        cv::Mat image = cv::imread(input_file);
        if (image.empty()) {
            cpp_engine::utils::Logger::instance().error("Failed to load image: " + input_file);
            return false;
        }

        cv::Mat contrast_adjusted;
        image.convertTo(contrast_adjusted, -1, factor, 0);

        if (cv::imwrite(output_file, contrast_adjusted)) {
            cpp_engine::utils::Logger::instance().info("Contrast adjusted successfully");
            return true;
        } else {
            cpp_engine::utils::Logger::instance().error("Failed to save contrast adjusted image: " + output_file);
            return false;
        }
    } catch (const cv::Exception& e) {
        cpp_engine::utils::Logger::instance().error("OpenCV error in contrast: " + std::string(e.what()));
        return false;
    }
}

bool ImageFilter::adjust_saturation(const std::string& input_file, const std::string& output_file, float factor) {
    try {
        cpp_engine::utils::Logger::instance().info("Adjusting saturation, factor=" + std::to_string(factor));

        cv::Mat image = cv::imread(input_file);
        if (image.empty()) {
            cpp_engine::utils::Logger::instance().error("Failed to load image: " + input_file);
            return false;
        }

        cv::Mat hsv_image;
        cv::cvtColor(image, hsv_image, cv::COLOR_BGR2HSV);

        // Ajuster la saturation (canal S)
        for (int i = 0; i < hsv_image.rows; ++i) {
            for (int j = 0; j < hsv_image.cols; ++j) {
                cv::Vec3b pixel = hsv_image.at<cv::Vec3b>(i, j);
                pixel[1] = cv::saturate_cast<uchar>(pixel[1] * factor);
                hsv_image.at<cv::Vec3b>(i, j) = pixel;
            }
        }

        cv::Mat saturation_adjusted;
        cv::cvtColor(hsv_image, saturation_adjusted, cv::COLOR_HSV2BGR);

        if (cv::imwrite(output_file, saturation_adjusted)) {
            cpp_engine::utils::Logger::instance().info("Saturation adjusted successfully");
            return true;
        } else {
            cpp_engine::utils::Logger::instance().error("Failed to save saturation adjusted image: " + output_file);
            return false;
        }
    } catch (const cv::Exception& e) {
        cpp_engine::utils::Logger::instance().error("OpenCV error in saturation: " + std::string(e.what()));
        return false;
    }
}

bool ImageFilter::detect_edges(const std::string& input_file, const std::string& output_file) {
    try {
        cpp_engine::utils::Logger::instance().info("Detecting edges with Canny");

        cv::Mat image = cv::imread(input_file);
        if (image.empty()) {
            cpp_engine::utils::Logger::instance().error("Failed to load image: " + input_file);
            return false;
        }

        cv::Mat gray, edges;
        cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
        cv::Canny(gray, edges, 100, 200);

        if (cv::imwrite(output_file, edges)) {
            cpp_engine::utils::Logger::instance().info("Edges detected successfully");
            return true;
        } else {
            cpp_engine::utils::Logger::instance().error("Failed to save edges image: " + output_file);
            return false;
        }
    } catch (const cv::Exception& e) {
        cpp_engine::utils::Logger::instance().error("OpenCV error in edge detection: " + std::string(e.what()));
        return false;
    }
}

bool ImageFilter::dilate(const std::string& input_file, const std::string& output_file, int kernel_size) {
    try {
        cpp_engine::utils::Logger::instance().info("Applying dilation, kernel=" + std::to_string(kernel_size));

        cv::Mat image = cv::imread(input_file);
        if (image.empty()) {
            cpp_engine::utils::Logger::instance().error("Failed to load image: " + input_file);
            return false;
        }

        cv::Mat dilated;
        cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(kernel_size, kernel_size));
        cv::dilate(image, dilated, kernel);

        if (cv::imwrite(output_file, dilated)) {
            cpp_engine::utils::Logger::instance().info("Dilation applied successfully");
            return true;
        } else {
            cpp_engine::utils::Logger::instance().error("Failed to save dilated image: " + output_file);
            return false;
        }
    } catch (const cv::Exception& e) {
        cpp_engine::utils::Logger::instance().error("OpenCV error in dilation: " + std::string(e.what()));
        return false;
    }
}

bool ImageFilter::erode(const std::string& input_file, const std::string& output_file, int kernel_size) {
    try {
        cpp_engine::utils::Logger::instance().info("Applying erosion, kernel=" + std::to_string(kernel_size));

        cv::Mat image = cv::imread(input_file);
        if (image.empty()) {
            cpp_engine::utils::Logger::instance().error("Failed to load image: " + input_file);
            return false;
        }

        cv::Mat eroded;
        cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(kernel_size, kernel_size));
        cv::erode(image, eroded, kernel);

        if (cv::imwrite(output_file, eroded)) {
            cpp_engine::utils::Logger::instance().info("Erosion applied successfully");
            return true;
        } else {
            cpp_engine::utils::Logger::instance().error("Failed to save eroded image: " + output_file);
            return false;
        }
    } catch (const cv::Exception& e) {
        cpp_engine::utils::Logger::instance().error("OpenCV error in erosion: " + std::string(e.what()));
        return false;
    }
}

} // namespace filters
} // namespace cppengine
