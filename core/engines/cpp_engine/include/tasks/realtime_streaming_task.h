#pragma once
/**
 * @class RealtimeStreamingTask
 * @brief Streaming temps-réel (WebRTC natif) — interface.
 *
 * Activer via flag CMake et implémenter WebRTC natif si nécessaire.
 */
#include <string>
#include <stdexcept>

class RealtimeStreamingTask {
public:
    RealtimeStreamingTask();
    ~RealtimeStreamingTask();
    void configure(const std::string& endpoint);
    bool run();
    void log(const std::string& message) const;
private:
    std::string endpoint_;
    bool configured_;
};
