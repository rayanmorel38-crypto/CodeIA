// feature_extractor.h
#pragma once

#include <vector>
#include <cstddef>

namespace cpp_engine::modules::vision {

class FeatureExtractor {
public:
    FeatureExtractor();
    ~FeatureExtractor();

    bool initialize();
    std::vector<float> extract(const unsigned char *rgb_data, int width, int height);
};

} // namespace
