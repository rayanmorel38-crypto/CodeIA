#pragma once
/**
 * @class RenderingTask
 * @brief Tâche de rendu (raytracing / GPU). Interface pour rendu haute qualité.
 */
#include <string>
#include <stdexcept>

class RenderingTask {
public:
    RenderingTask();
    ~RenderingTask();
    void configure(const std::string& renderer);
    bool run(const std::string& sceneFile, const std::string& outImage);
    void log(const std::string& message) const;
private:
    std::string renderer_;
    bool configured_;
};
