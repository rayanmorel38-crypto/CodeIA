#include "../../include/tasks/hashing_task.h"
#include <iostream>
#include <openssl/sha.h>

HashingTask::HashingTask() : algorithm_("sha256"), configured_(false) {}
HashingTask::~HashingTask() {}

void HashingTask::configure(const std::string& algorithm) {
    algorithm_ = algorithm;
    configured_ = true;
    log("Algorithm: " + algorithm_);
}

std::vector<uint8_t> HashingTask::run(const std::vector<uint8_t>& input) {
    if (!configured_) throw std::runtime_error("HashingTask non configurée");
    log("Calcul du hash (simulation/openssl si dispo)");
    // Tentative d'utiliser OpenSSL SHA256 si disponible, sinon retour vide
#ifdef HAVE_OPENSSL
    std::vector<uint8_t> out(SHA256_DIGEST_LENGTH);
    SHA256(input.data(), input.size(), out.data());
    return out;
#else
    // Fallback simulé: retourne la taille en bytes
    std::vector<uint8_t> out(4);
    uint32_t v = (uint32_t)input.size();
    out[0] = v & 0xFF; out[1] = (v>>8)&0xFF; out[2] = (v>>16)&0xFF; out[3] = (v>>24)&0xFF;
    return out;
#endif
}

void HashingTask::log(const std::string& message) const {
    std::cout << "[HashingTask] " << message << std::endl;
}
