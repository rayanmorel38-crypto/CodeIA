#include "../../include/tasks/rendering_task.h"
#include "utils/logger.h"

RenderingTask::RenderingTask() : configured_(false) {}
RenderingTask::~RenderingTask() {}

void RenderingTask::configure(const std::string& renderer) {
    renderer_ = renderer;
    configured_ = true;
    log("Renderer configured: " + renderer_);
}

bool RenderingTask::run(const std::string& sceneFile, const std::string& outImage) {
    if (!configured_) throw std::runtime_error("RenderingTask non configurée");
    log("Rendering scene=" + sceneFile + " -> " + outImage);
#ifdef USE_GPU_RENDERER
    log("GPU renderer path (placeholder)");
    return true;
#else
    log("CPU fallback render (simulation)");
    return true;
#endif
}

void RenderingTask::log(const std::string& message) const {
    cpp_engine::utils::Logger::instance().info(std::string("[RenderingTask] ") + message);
}
