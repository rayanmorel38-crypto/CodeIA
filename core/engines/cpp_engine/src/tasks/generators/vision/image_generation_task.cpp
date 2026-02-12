// Image generation task (robust placeholder implementation)
#include "../../include/tasks/image_generation_task.h"
#include <iostream>
#include <fstream>
#include "utils/logger.h"

ImageGenerationTask::ImageGenerationTask() : configured_(false) {}
ImageGenerationTask::~ImageGenerationTask() {}

void ImageGenerationTask::configure(const std::string& modelPath) {
    modelPath_ = modelPath;
    configured_ = true;
    log("Configured model=" + modelPath_);
}

std::vector<uint8_t> ImageGenerationTask::run(const std::string& prompt) {
    if (!configured_) throw std::runtime_error("ImageGenerationTask non configurée");
    log("Generating image for prompt: " + prompt);
    // Simulated PNG bytes: here we return a tiny PNG header (not a valid image but demonstrative)
    const char* fakePng = "\x89PNG\r\n\x1a\n";
    std::vector<uint8_t> out(fakePng, fakePng + 8);
    log("Image generation completed, bytes=" + std::to_string((int)out.size()));
    return out;
}

bool ImageGenerationTask::saveToFile(const std::string& outPath, const std::vector<uint8_t>& bytes) {
    std::ofstream ofs(outPath, std::ios::binary);
    if (!ofs) return false;
    ofs.write(reinterpret_cast<const char*>(bytes.data()), bytes.size());
    return true;
}

void ImageGenerationTask::log(const std::string& message) const {
    cpp_engine::utils::Logger::instance().info("[ImageGenerationTask] " + message);
}
