// vision_module.cpp -- aggregator and simple pipeline
#include "modules/vision/vision_module.h"
#include <iostream>

namespace cpp_engine::modules::vision {

VisionModule::VisionModule() {}
VisionModule::~VisionModule() { stop(); }

bool VisionModule::start() { return true; }
void VisionModule::stop() {}

void VisionModule::process_frame(const unsigned char *rgb_data, int width, int height) {
    (void)rgb_data; (void)width; (void)height;
    // In a real implementation run detector/classifier/feature_extractor
    std::cout << "vision_module: processed frame (stub)\n";
}

bool VisionModule::attach_kinect() {
    // In real code create KinectInterface and start
    return true;
}

void VisionModule::detach_kinect() {}

} // namespace cpp_engine::modules::vision
