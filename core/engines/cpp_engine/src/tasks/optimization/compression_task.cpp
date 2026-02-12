#include "../../include/tasks/compression_task.h"
#include <iostream>

CompressionTask::CompressionTask() : level_(3), configured_(false) {}
CompressionTask::~CompressionTask() {}

void CompressionTask::configure(int level) {
    level_ = level;
    configured_ = true;
    log("Compression configured, level=" + std::to_string(level_));
}

std::vector<uint8_t> CompressionTask::run(const std::vector<uint8_t>& input) {
    if (!configured_) throw std::runtime_error("CompressionTask non configurée");
    log("Début compression (simulation)\n");
    // Simulation très simple: copie donnée (remplacer par impl SIMD réelle)
    std::vector<uint8_t> out = input;
    return out;
}

void CompressionTask::log(const std::string& message) const {
    std::cout << "[CompressionTask] " << message << std::endl;
}
