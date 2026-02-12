#include "../../include/tasks/decompression_task.h"
#include <iostream>

DecompressionTask::DecompressionTask() : configured_(false) {}
DecompressionTask::~DecompressionTask() {}

void DecompressionTask::configure() {
    configured_ = true;
    log("Decompression configured");
}

std::vector<uint8_t> DecompressionTask::run(const std::vector<uint8_t>& input) {
    if (!configured_) throw std::runtime_error("DecompressionTask non configurée");
    log("Début décompression (simulation)");
    // Simulation: retourne identiquement les données
    std::vector<uint8_t> out = input;
    return out;
}

void DecompressionTask::log(const std::string& message) const {
    std::cout << "[DecompressionTask] " << message << std::endl;
}
