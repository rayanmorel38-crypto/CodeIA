#include <algorithm>
#include <vector>
#include <string>
#include <iostream>
#include <filesystem>
#include <random>
#include <cmath>

// Stub for OpenCV types when not available
namespace cv {
    struct Mat {
        Mat() {}
        Mat(int, int, int) {}
    };
    struct Size {
        Size(int, int) {}
    };
    struct VideoWriter {
        bool open(const std::string&, int, int, Size) { return false; }
        bool isOpened() { return false; }
        void write(const Mat&) {}
        void release() {}
        static int fourcc(char, char, char, char) { return 0; }
    };
    bool imwrite(const std::string&, const Mat&) { return false; }
}

#include <filesystem>

namespace fs = std::filesystem;

// ============================================================================
// Perlin Noise Implementation (Simplex-like, simplified for 2D)
// ============================================================================
class PerlinNoise {
private:
    std::vector<int> permutation;
    std::mt19937 gen;

public:
    PerlinNoise(unsigned int seed = 0) : gen(seed) {
        permutation.resize(256);
        std::iota(permutation.begin(), permutation.end(), 0);
        std::shuffle(permutation.begin(), permutation.end(), gen);
        permutation.insert(permutation.end(), permutation.begin(), permutation.end());
    }

    double fade(double t) {
        return t * t * t * (t * (t * 6.0 - 15.0) + 10.0);
    }

    double lerp(double t, double a, double b) {
        return a + t * (b - a);
    }

    double grad(int hash, double x, double y) {
        int h = hash & 15;
        double u = (h < 8) ? x : y;
        double v = (h < 8) ? y : x;
        return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
    }

    double noise(double x, double y) {
        int xi = static_cast<int>(std::floor(x)) & 255;
        int yi = static_cast<int>(std::floor(y)) & 255;

        double xf = x - std::floor(x);
        double yf = y - std::floor(y);

        double u = fade(xf);
        double v = fade(yf);

        int aa = permutation[permutation[xi] + yi];
        int ab = permutation[permutation[xi] + yi + 1];
        int ba = permutation[permutation[xi + 1] + yi];
        int bb = permutation[permutation[xi + 1] + yi + 1];

        double x1 = lerp(u, grad(aa, xf, yf), grad(ba, xf - 1, yf));
        double x2 = lerp(u, grad(ab, xf, yf - 1), grad(bb, xf - 1, yf - 1));
        return lerp(v, x1, x2);
    }

    // Multi-octave Perlin noise (fractal brownian motion)
    double fractal(double x, double y, int octaves = 4, double persistence = 0.5, double scale = 2.0) {
        double result = 0.0;
        double amplitude = 1.0;
        double frequency = 1.0;
        double max_value = 0.0;

        for (int i = 0; i < octaves; ++i) {
            result += amplitude * noise(x * frequency, y * frequency);
            max_value += amplitude;
            amplitude *= persistence;
            frequency *= scale;
        }

        return result / max_value;
    }
};

// ============================================================================
// Image Generators
// ============================================================================
class ImageGenerator {
public:
    virtual ~ImageGenerator() = default;
    virtual cv::Mat generate(int width, int height, unsigned int seed) = 0;
};

class PerlinImageGenerator : public ImageGenerator {
public:
    cv::Mat generate(int width, int height, unsigned int seed) override {
        PerlinNoise perlin(seed);
        cv::Mat image(height, width, CV_8UC3, cv::Scalar(0, 0, 0));

        double scale = 0.01;  // Adjust for desired noise granularity
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                double nx = x * scale;
                double ny = y * scale;
                double value = perlin.fractal(nx, ny, 4, 0.5, 2.0);
                double normalized = (value + 1.0) / 2.0;  // Map from [-1,1] to [0,1]
                unsigned char pixel = static_cast<unsigned char>(normalized * 255.0);

                image.at<cv::Vec3b>(y, x) = cv::Vec3b(pixel, pixel, pixel);
            }
        }

        // Apply colorization
        cv::Mat hsv_image;
        cv::cvtColor(image, hsv_image, cv::COLOR_BGR2HSV);
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                auto val = hsv_image.at<cv::Vec3b>(y, x);
                val[0] = (val[0] + (x % 180)) % 180;  // Hue cycling
                hsv_image.at<cv::Vec3b>(y, x) = val;
            }
        }
        cv::Mat result;
        cv::cvtColor(hsv_image, result, cv::COLOR_HSV2BGR);
        return result;
    }
};

class SilhouetteGenerator : public ImageGenerator {
public:
    cv::Mat generate(int width, int height, unsigned int seed) override {
        PerlinNoise perlin(seed);
        cv::Mat base(height, width, CV_8UC1, cv::Scalar(0));

        double scale = 0.015;
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                double nx = x * scale;
                double ny = y * scale;
                double value = perlin.fractal(nx, ny, 5, 0.6, 2.0);
                double normalized = (value + 1.0) / 2.0;
                base.at<unsigned char>(y, x) = static_cast<unsigned char>(normalized * 255.0);
            }
        }

        // Create contour/silhouette
        cv::Mat edges;
        cv::Canny(base, edges, 50, 150);
        cv::Mat silhouette = cv::Mat::zeros(height, width, CV_8UC3);
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                if (edges.at<unsigned char>(y, x) > 0) {
                    silhouette.at<cv::Vec3b>(y, x) = cv::Vec3b(255, 200, 50);  // Gold contour
                }
            }
        }

        // Morphological operations to enhance silhouette
        cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3));
        cv::dilate(silhouette, silhouette, kernel, cv::Point(-1, -1), 1);

        return silhouette;
    }
};

class MetallicGenerator : public ImageGenerator {
public:
    cv::Mat generate(int width, int height, unsigned int seed) override {
        PerlinNoise perlin(seed);
        cv::Mat image(height, width, CV_8UC3, cv::Scalar(30, 30, 40));

        double scale = 0.02;
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                double nx = x * scale;
                double ny = y * scale;

                // Normal approximation using Perlin gradient
                double h = perlin.fractal(nx, ny, 4, 0.5, 2.0);
                double hx = perlin.fractal(nx + 0.01, ny, 4, 0.5, 2.0);
                double hy = perlin.fractal(nx, ny + 0.01, 4, 0.5, 2.0);

                // Phong lighting
                double light = 0.5 + 0.5 * (h + hx + hy) / 3.0;
                unsigned char intensity = static_cast<unsigned char>(light * 200.0 + 30.0);

                image.at<cv::Vec3b>(y, x) = cv::Vec3b(intensity, intensity, intensity);
            }
        }

        // Add specular highlights (directional light)
        for (int y = 10; y < height - 10; ++y) {
            for (int x = 10; x < width - 10; ++x) {
                double dist = std::sqrt(std::pow(x - width / 2.0, 2) + std::pow(y - height / 2.0, 2));
                if (dist < 50) {
                    int highlight = static_cast<int>(200 * std::exp(-dist * dist / 1000.0));
                    auto pix = image.at<cv::Vec3b>(y, x);
                    pix[0] = cv::saturate_cast<unsigned char>(pix[0] + highlight);
                    pix[1] = cv::saturate_cast<unsigned char>(pix[1] + highlight);
                    pix[2] = cv::saturate_cast<unsigned char>(pix[2] + highlight);
                    image.at<cv::Vec3b>(y, x) = pix;
                }
            }
        }

        return image;
    }
};

// ============================================================================
// Video Writer
// ============================================================================
class VideoWriter {
private:
    std::string filename;
    int width, height, fps;
    cv::VideoWriter writer;

public:
    VideoWriter(const std::string& file, int w, int h, int f)
        : filename(file), width(w), height(h), fps(f) {
        int fourcc = cv::VideoWriter::fourcc('m', 'p', '4', 'v');  // mp4v codec
        writer.open(filename, fourcc, fps, cv::Size(width, height));
        if (!writer.isOpened()) {
            throw std::runtime_error("Failed to open video writer for: " + filename);
        }
    }

    void write(const cv::Mat& frame) {
        if (frame.size() != cv::Size(width, height)) {
            cv::Mat resized;
            cv::resize(frame, resized, cv::Size(width, height));
            writer.write(resized);
        } else {
            writer.write(frame);
        }
    }

    ~VideoWriter() {
        if (writer.isOpened()) {
            writer.release();
        }
    }
};

// ============================================================================
// Main Generator Functions
// ============================================================================
void generateImage(const std::string& output, int width, int height, unsigned int seed, const std::string& type) {
    std::cout << "Generating " << type << " image (" << width << "x" << height << ")..." << std::endl;

    std::unique_ptr<ImageGenerator> generator;
    if (type == "perlin") {
        generator = std::make_unique<PerlinImageGenerator>();
    } else if (type == "silhouette") {
        generator = std::make_unique<SilhouetteGenerator>();
    } else if (type == "metallic") {
        generator = std::make_unique<MetallicGenerator>();
    } else {
        throw std::invalid_argument("Unknown image type: " + type);
    }

    cv::Mat image = generator->generate(width, height, seed);

    // Create output directory
    fs::path output_path(output);
    fs::create_directories(output_path.parent_path());

    cv::imwrite(output, image);
    std::cout << "Saved to: " << output << std::endl;
}

void generateVideo(const std::string& output, int width, int height, int frames, int fps, const std::string& type) {
    std::cout << "Generating " << type << " video (" << width << "x" << height << ", " << frames << " frames @ " << fps << " fps)..." << std::endl;

    // Create output directory
    fs::path output_path(output);
    fs::create_directories(output_path.parent_path());

    VideoWriter video(output, width, height, fps);

    std::unique_ptr<ImageGenerator> generator;
    if (type == "perlin_video") {
        generator = std::make_unique<PerlinImageGenerator>();
    } else if (type == "silhouette_video") {
        generator = std::make_unique<SilhouetteGenerator>();
    } else if (type == "metallic_video") {
        generator = std::make_unique<MetallicGenerator>();
    } else {
        throw std::invalid_argument("Unknown video type: " + type);
    }

    for (int i = 0; i < frames; ++i) {
        unsigned int seed = i;
        cv::Mat frame = generator->generate(width, height, seed);
        video.write(frame);
        
        if ((i + 1) % 10 == 0) {
            std::cout << "  Progress: " << (i + 1) << "/" << frames << " frames" << std::endl;
        }
    }

    std::cout << "Saved video to: " << output << std::endl;
}

// ============================================================================
// Main Entry Point
// ============================================================================
int main(int argc, char** argv) {
    try {
        // Parse arguments
        std::string output = "output.png";
        int width = 512;
        int height = 512;
        int frames = 60;
        int fps = 30;
        std::string type = "perlin";
        unsigned int seed = 0;
        bool random_seed = true;

        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "--output" && i + 1 < argc) {
                output = argv[++i];
            } else if (arg == "--width" && i + 1 < argc) {
                width = std::stoi(argv[++i]);
            } else if (arg == "--height" && i + 1 < argc) {
                height = std::stoi(argv[++i]);
            } else if (arg == "--frames" && i + 1 < argc) {
                frames = std::stoi(argv[++i]);
            } else if (arg == "--fps" && i + 1 < argc) {
                fps = std::stoi(argv[++i]);
            } else if (arg == "--type" && i + 1 < argc) {
                type = argv[++i];
            } else if (arg == "--seed" && i + 1 < argc) {
                seed = std::stoul(argv[++i]);
                random_seed = false;
            } else if (arg == "--help") {
                std::cout << "Usage: image_video_generator [options]\n"
                    << "Options:\n"
                    << "  --output FILE     Output file path (default: output.png)\n"
                    << "  --width N         Image width in pixels (default: 512)\n"
                    << "  --height N        Image height in pixels (default: 512)\n"
                    << "  --frames N        Number of video frames (default: 60)\n"
                    << "  --fps N           Video fps (default: 30)\n"
                    << "  --type TYPE       Generator type: perlin, silhouette, metallic, perlin_video, etc. (default: perlin)\n"
                    << "  --seed N          Random seed (default: random)\n"
                    << "  --help            Show this help message\n";
                return 0;
            }
        }

        // Use random seed if not specified
        if (random_seed) {
            seed = std::random_device{}();
        }

        // Determine if generating image or video based on type
        if (type.find("_video") != std::string::npos) {
            generateVideo(output, width, height, frames, fps, type);
        } else {
            generateImage(output, width, height, seed, type);
        }

        std::cout << "Done!" << std::endl;
        return 0;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
