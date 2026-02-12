// classifier.h
#pragma once

#include <string>
#include <vector>

namespace cpp_engine::modules::vision {

class Classifier {
public:
    Classifier();
    ~Classifier();

    bool load(const std::string &model_path);
    std::vector<std::pair<std::string, float>> classify(const unsigned char *rgb_data, int width, int height);
};

} // namespace
