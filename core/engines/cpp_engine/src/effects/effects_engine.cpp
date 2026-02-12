#include "effects/effects_engine.h"
#include "utils/logger.h"
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <cmath>
#include <random>

namespace cppengine {
namespace effects {

EffectsEngine::EffectsEngine() : effect_quality_(5) {
    cpp_engine::utils::Logger::instance().info("EffectsEngine initialized with OpenCV");
}

EffectsEngine::~EffectsEngine() {}

bool EffectsEngine::apply_lighting(const std::string& input_file, const std::string& output_file,
                                  float light_x, float light_y, float light_z) {
    try {
        cpp_engine::utils::Logger::instance().info("Applying 3D lighting effects");

        cv::Mat image = cv::imread(input_file);
        if (image.empty()) {
            cpp_engine::utils::Logger::instance().error("Failed to load image: " + input_file);
            return false;
        }

        cv::Mat result = image.clone();

        // Simuler un éclairage directionnel 3D
        cv::Vec3f light_dir(light_x, light_y, light_z);
        cv::normalize(light_dir, light_dir);

        // Calculer les normales de surface approximatives
        cv::Mat gray, grad_x, grad_y, normal_map;
        cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
        cv::Sobel(gray, grad_x, CV_32F, 1, 0, 3);
        cv::Sobel(gray, grad_y, CV_32F, 0, 1, 3);

        // Appliquer l'éclairage
        for (int i = 0; i < result.rows; ++i) {
            for (int j = 0; j < result.cols; ++j) {
                cv::Vec3f normal(0, 0, 1);
                if (i > 0 && i < grad_x.rows && j > 0 && j < grad_x.cols) {
                    normal[0] = grad_x.at<float>(i, j) / 255.0f;
                    normal[1] = grad_y.at<float>(i, j) / 255.0f;
                }
                cv::normalize(normal, normal);

                float dot_product = normal.dot(light_dir);
                float intensity = std::max(0.3f, dot_product * 0.7f + 0.3f);

                cv::Vec3b pixel = result.at<cv::Vec3b>(i, j);
                pixel[0] = cv::saturate_cast<uchar>(pixel[0] * intensity);
                pixel[1] = cv::saturate_cast<uchar>(pixel[1] * intensity);
                pixel[2] = cv::saturate_cast<uchar>(pixel[2] * intensity);
                result.at<cv::Vec3b>(i, j) = pixel;
            }
        }

        if (cv::imwrite(output_file, result)) {
            cpp_engine::utils::Logger::instance().info("3D lighting applied successfully");
            return true;
        } else {
            cpp_engine::utils::Logger::instance().error("Failed to save lit image: " + output_file);
            return false;
        }
    } catch (const cv::Exception& e) {
        cpp_engine::utils::Logger::instance().error("OpenCV error in lighting: " + std::string(e.what()));
        return false;
    }
}

bool EffectsEngine::apply_shadows(const std::string& input_file, const std::string& output_file,
                                 float shadow_intensity) {
    try {
        cpp_engine::utils::Logger::instance().info("Applying shadow effects, intensity=" + std::to_string(shadow_intensity));

        cv::Mat image = cv::imread(input_file);
        if (image.empty()) {
            cpp_engine::utils::Logger::instance().error("Failed to load image: " + input_file);
            return false;
        }

        cv::Mat result = image.clone();

        // Créer une ombre directionnelle
        cv::Mat shadow_mask = cv::Mat::zeros(image.size(), CV_8UC1);
        cv::rectangle(shadow_mask, cv::Rect(image.cols/4, image.rows/4, image.cols/2, image.rows/2),
                     cv::Scalar(255), -1);

        // Appliquer un flou gaussien pour adoucir l'ombre
        cv::GaussianBlur(shadow_mask, shadow_mask, cv::Size(21, 21), 0);

        // Appliquer l'ombre
        for (int i = 0; i < result.rows; ++i) {
            for (int j = 0; j < result.cols; ++j) {
                float shadow_factor = shadow_mask.at<uchar>(i, j) / 255.0f * shadow_intensity;
                cv::Vec3b pixel = result.at<cv::Vec3b>(i, j);
                pixel[0] = cv::saturate_cast<uchar>(pixel[0] * (1.0f - shadow_factor * 0.5f));
                pixel[1] = cv::saturate_cast<uchar>(pixel[1] * (1.0f - shadow_factor * 0.5f));
                pixel[2] = cv::saturate_cast<uchar>(pixel[2] * (1.0f - shadow_factor * 0.5f));
                result.at<cv::Vec3b>(i, j) = pixel;
            }
        }

        if (cv::imwrite(output_file, result)) {
            cpp_engine::utils::Logger::instance().info("Shadow effects applied successfully");
            return true;
        } else {
            cpp_engine::utils::Logger::instance().error("Failed to save shadowed image: " + output_file);
            return false;
        }
    } catch (const cv::Exception& e) {
        cpp_engine::utils::Logger::instance().error("OpenCV error in shadows: " + std::string(e.what()));
        return false;
    }
}

bool EffectsEngine::add_particles(const std::string& input_file, const std::string& output_file,
                                 int particle_count, const std::string& particle_type) {
    try {
        cpp_engine::utils::Logger::instance().info("Adding " + std::to_string(particle_count) + " " + particle_type + " particles");

        cv::Mat image = cv::imread(input_file);
        if (image.empty()) {
            cpp_engine::utils::Logger::instance().error("Failed to load image: " + input_file);
            return false;
        }

        cv::Mat result = image.clone();
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> x_dist(0, image.cols - 1);
        std::uniform_int_distribution<> y_dist(0, image.rows - 1);

        // Générer des particules selon le type
        cv::Scalar particle_color;
        int particle_size = 2;

        if (particle_type == "fire") {
            particle_color = cv::Scalar(0, 69, 255); // Rouge-orange
        } else if (particle_type == "water") {
            particle_color = cv::Scalar(255, 191, 0); // Bleu
        } else if (particle_type == "spark") {
            particle_color = cv::Scalar(0, 255, 255); // Jaune
            particle_size = 1;
        } else {
            particle_color = cv::Scalar(255, 255, 255); // Blanc par défaut
        }

        for (int i = 0; i < particle_count; ++i) {
            int x = x_dist(gen);
            int y = y_dist(gen);
            cv::circle(result, cv::Point(x, y), particle_size, particle_color, -1);
        }

        if (cv::imwrite(output_file, result)) {
            cpp_engine::utils::Logger::instance().info("Particles added successfully");
            return true;
        } else {
            cpp_engine::utils::Logger::instance().error("Failed to save particle image: " + output_file);
            return false;
        }
    } catch (const cv::Exception& e) {
        cpp_engine::utils::Logger::instance().error("OpenCV error in particles: " + std::string(e.what()));
        return false;
    }
}

bool EffectsEngine::apply_wave_distortion(const std::string& input_file, const std::string& output_file,
                                         float amplitude, float frequency) {
    try {
        cpp_engine::utils::Logger::instance().info("Applying wave distortion, amp=" + std::to_string(amplitude) +
                                                  ", freq=" + std::to_string(frequency));

        cv::Mat image = cv::imread(input_file);
        if (image.empty()) {
            cpp_engine::utils::Logger::instance().error("Failed to load image: " + input_file);
            return false;
        }

        cv::Mat result = cv::Mat::zeros(image.size(), image.type());

        // Appliquer une distorsion sinusoïdale
        for (int i = 0; i < image.rows; ++i) {
            for (int j = 0; j < image.cols; ++j) {
                int offset_x = static_cast<int>(amplitude * sin(2 * M_PI * frequency * i / image.rows));
                int offset_y = static_cast<int>(amplitude * cos(2 * M_PI * frequency * j / image.cols));

                int src_x = j + offset_x;
                int src_y = i + offset_y;

                if (src_x >= 0 && src_x < image.cols && src_y >= 0 && src_y < image.rows) {
                    result.at<cv::Vec3b>(i, j) = image.at<cv::Vec3b>(src_y, src_x);
                } else {
                    result.at<cv::Vec3b>(i, j) = cv::Vec3b(0, 0, 0);
                }
            }
        }

        if (cv::imwrite(output_file, result)) {
            cpp_engine::utils::Logger::instance().info("Wave distortion applied successfully");
            return true;
        } else {
            cpp_engine::utils::Logger::instance().error("Failed to save distorted image: " + output_file);
            return false;
        }
    } catch (const cv::Exception& e) {
        cpp_engine::utils::Logger::instance().error("OpenCV error in wave distortion: " + std::string(e.what()));
        return false;
    }
}

bool EffectsEngine::apply_radial_distortion(const std::string& input_file, const std::string& output_file,
                                           float distortion_factor) {
    try {
        cpp_engine::utils::Logger::instance().info("Applying radial distortion, factor=" + std::to_string(distortion_factor));

        cv::Mat image = cv::imread(input_file);
        if (image.empty()) {
            cpp_engine::utils::Logger::instance().error("Failed to load image: " + input_file);
            return false;
        }

        cv::Mat result = cv::Mat::zeros(image.size(), image.type());
        cv::Point2f center(image.cols / 2.0f, image.rows / 2.0f);
        float max_radius = std::sqrt(center.x * center.x + center.y * center.y);

        for (int i = 0; i < image.rows; ++i) {
            for (int j = 0; j < image.cols; ++j) {
                cv::Point2f point(j, i);
                cv::Point2f vec = point - center;
                float radius = cv::norm(vec);

                if (radius > 0) {
                    float distortion = 1.0f + distortion_factor * (radius / max_radius) * (radius / max_radius);
                    cv::Point2f src_point = center + vec / distortion;

                    if (src_point.x >= 0 && src_point.x < image.cols - 1 &&
                        src_point.y >= 0 && src_point.y < image.rows - 1) {
                        result.at<cv::Vec3b>(i, j) = image.at<cv::Vec3b>(cv::saturate_cast<int>(src_point.y),
                                                                       cv::saturate_cast<int>(src_point.x));
                    }
                }
            }
        }

        if (cv::imwrite(output_file, result)) {
            cpp_engine::utils::Logger::instance().info("Radial distortion applied successfully");
            return true;
        } else {
            cpp_engine::utils::Logger::instance().error("Failed to save distorted image: " + output_file);
            return false;
        }
    } catch (const cv::Exception& e) {
        cpp_engine::utils::Logger::instance().error("OpenCV error in radial distortion: " + std::string(e.what()));
        return false;
    }
}

bool EffectsEngine::apply_chromatic_aberration(const std::string& input_file, const std::string& output_file,
                                              float red_shift, float blue_shift) {
    try {
        cpp_engine::utils::Logger::instance().info("Applying chromatic aberration, red_shift=" + std::to_string(red_shift) +
                                                  ", blue_shift=" + std::to_string(blue_shift));

        cv::Mat image = cv::imread(input_file);
        if (image.empty()) {
            cpp_engine::utils::Logger::instance().error("Failed to load image: " + input_file);
            return false;
        }

        std::vector<cv::Mat> channels;
        cv::split(image, channels);

        // Décaler les canaux rouge et bleu
        cv::Mat red_shifted, blue_shifted;
        cv::warpAffine(channels[2], red_shifted, cv::Mat::eye(2, 3, CV_32F), image.size(),
                      cv::INTER_LINEAR, cv::BORDER_CONSTANT, cv::Scalar(0));
        cv::warpAffine(channels[0], blue_shifted, cv::Mat::eye(2, 3, CV_32F), image.size(),
                      cv::INTER_LINEAR, cv::BORDER_CONSTANT, cv::Scalar(0));

        // Translation pour l'aberration chromatique
        cv::Mat translation_red = (cv::Mat_<float>(2, 3) << 1, 0, red_shift, 0, 1, 0);
        cv::Mat translation_blue = (cv::Mat_<float>(2, 3) << 1, 0, -blue_shift, 0, 1, 0);

        cv::warpAffine(channels[2], red_shifted, translation_red, image.size());
        cv::warpAffine(channels[0], blue_shifted, translation_blue, image.size());

        // Recombinaison
        std::vector<cv::Mat> aberrated_channels = {blue_shifted, channels[1], red_shifted};
        cv::Mat result;
        cv::merge(aberrated_channels, result);

        if (cv::imwrite(output_file, result)) {
            cpp_engine::utils::Logger::instance().info("Chromatic aberration applied successfully");
            return true;
        } else {
            cpp_engine::utils::Logger::instance().error("Failed to save aberrated image: " + output_file);
            return false;
        }
    } catch (const cv::Exception& e) {
        cpp_engine::utils::Logger::instance().error("OpenCV error in chromatic aberration: " + std::string(e.what()));
        return false;
    }
}

bool EffectsEngine::apply_bloom(const std::string& input_file, const std::string& output_file,
                               float threshold, float intensity) {
    try {
        cpp_engine::utils::Logger::instance().info("Applying bloom effect, threshold=" + std::to_string(threshold) +
                                                  ", intensity=" + std::to_string(intensity));

        cv::Mat image = cv::imread(input_file);
        if (image.empty()) {
            cpp_engine::utils::Logger::instance().error("Failed to load image: " + input_file);
            return false;
        }

        // Convertir en float pour les calculs
        cv::Mat float_image;
        image.convertTo(float_image, CV_32FC3, 1.0/255.0);

        // Extraire les zones lumineuses
        cv::Mat bright_areas;
        cv::threshold(float_image, bright_areas, threshold, 1.0, cv::THRESH_BINARY);

        // Appliquer un flou gaussien pour créer l'effet de bloom
        cv::Mat bloom;
        cv::GaussianBlur(bright_areas, bloom, cv::Size(21, 21), 0);

        // Combiner l'image originale avec l'effet bloom
        cv::Mat result;
        cv::addWeighted(float_image, 1.0, bloom, intensity, 0.0, result);

        // Reconvertir en 8-bit
        result.convertTo(result, CV_8UC3, 255.0);

        if (cv::imwrite(output_file, result)) {
            cpp_engine::utils::Logger::instance().info("Bloom effect applied successfully");
            return true;
        } else {
            cpp_engine::utils::Logger::instance().error("Failed to save bloom image: " + output_file);
            return false;
        }
    } catch (const cv::Exception& e) {
        cpp_engine::utils::Logger::instance().error("OpenCV error in bloom: " + std::string(e.what()));
        return false;
    }
}

} // namespace effects
} // namespace cppengine
