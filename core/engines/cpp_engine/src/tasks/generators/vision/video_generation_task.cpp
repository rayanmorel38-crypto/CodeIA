// Video generation task (robust placeholder)
#include "../../include/tasks/video_generation_task.h"
#include "utils/logger.h"
#include <fstream>

VideoGenerationTask::VideoGenerationTask() : configured_(false) {}
VideoGenerationTask::~VideoGenerationTask() {}

void VideoGenerationTask::configure(const std::string& modelPath) {
    modelPath_ = modelPath;
    configured_ = true;
    log("Configured video model=" + modelPath_);
}

std::vector<uint8_t> VideoGenerationTask::run(const std::string& prompt) {
    if (!configured_) throw std::runtime_error("VideoGenerationTask non configurée");
    log("Generating video for: " + prompt);
    // Simulated container header bytes
    const char* fakeMp4 = "\x00\x00\x00\x18ftypmp42";
    std::vector<uint8_t> out(fakeMp4, fakeMp4 + 12);
    log("Video generation completed, bytes=" + std::to_string((int)out.size()));
    return out;
}

bool VideoGenerationTask::saveToFile(const std::string& outPath, const std::vector<uint8_t>& bytes) {
    std::ofstream ofs(outPath, std::ios::binary);
    if (!ofs) return false;
    ofs.write(reinterpret_cast<const char*>(bytes.data()), bytes.size());
    return true;
}

void VideoGenerationTask::log(const std::string& message) const {
    cpp_engine::utils::Logger::instance().info(std::string("[VideoGenerationTask] ") + message);
}
