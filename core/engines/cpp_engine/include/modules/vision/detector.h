// detector.h
#pragma once

#include <string>
#include <vector>
#include <map>

namespace cpp_engine::modules::vision {

struct Detection {
    std::string label;
    float confidence;
    int x, y, w, h; // bounding box
};

class Detector {
public:
    Detector();
    ~Detector();

    bool initialize(const std::string &model_path = "");
    std::vector<Detection> detect(const unsigned char *rgb_data, int width, int height);
    void reset();
};

} // namespace
