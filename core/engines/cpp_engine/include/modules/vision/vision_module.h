// vision_module.h
#pragma once

#include <string>
#include "detector.h"
#include "classifier.h"
#include "feature_extractor.h"
#include "kinect_interface.h"

namespace cpp_engine::modules::vision {

class VisionModule {
public:
    VisionModule();
    ~VisionModule();

    bool start();
    void stop();

    // Process a single frame (RGB bytes)
    void process_frame(const unsigned char *rgb_data, int width, int height);

    // Kinect access
    bool attach_kinect();
    void detach_kinect();
};

} // namespace
