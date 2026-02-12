#include "../../include/tasks/realtime_streaming_task.h"
#include <iostream>

RealtimeStreamingTask::RealtimeStreamingTask() : configured_(false) {}
RealtimeStreamingTask::~RealtimeStreamingTask() {}

void RealtimeStreamingTask::configure(const std::string& endpoint) {
    endpoint_ = endpoint;
    configured_ = true;
    log("Endpoint: " + endpoint_);
}

bool RealtimeStreamingTask::run() {
    if (!configured_) throw std::runtime_error("RealtimeStreamingTask non configurée");
    log("Démarrage du streaming (simulation)");
#ifdef USE_WEBRTC
    log("WebRTC path (placeholder)");
    return true;
#else
    log("WebRTC non activé: mode simulé");
    return false;
#endif
}

void RealtimeStreamingTask::log(const std::string& message) const {
    std::cout << "[RealtimeStreamingTask] " << message << std::endl;
}
