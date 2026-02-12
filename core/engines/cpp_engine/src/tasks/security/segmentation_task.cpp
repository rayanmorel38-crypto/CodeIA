#include "../../include/tasks/segmentation_task.h"
#include <iostream>

SegmentationTask::SegmentationTask() : configured_(false) {}
SegmentationTask::~SegmentationTask() {}

void SegmentationTask::configure(const std::string& model) {
    model_ = model; configured_ = true; log("Model: " + model_);
}

std::vector<uint8_t> SegmentationTask::run(const std::string& imagePath) {
    if (!configured_) throw std::runtime_error("SegmentationTask non configurée");
    log("Segmenting: " + imagePath);
    // Simulated mask of 10x10
    std::vector<uint8_t> mask(100, 0);
    // set central region to 255
    for (int y = 3; y < 7; ++y) for (int x = 3; x < 7; ++x) mask[y*10 + x] = 255;
    return mask;
}

void SegmentationTask::log(const std::string& message) const {
    std::cout << "[SegmentationTask] " << message << std::endl;
}
