#pragma once
/**
 * @class CompressionTask
 * @brief Compression optimisée (simulée). Implémentation prévue SIMD-friendly.
 *
 * Note: l'implémentation réelle doit utiliser des bibliothèques SIMD (e.g. x86 intrinsics)
 * ou libz/liblz4/libzstd selon cible. Ici on fournit une interface robuste et des points d'extension.
 */
#include <string>
#include <vector>
#include <stdexcept>

class CompressionTask {
public:
    CompressionTask();
    ~CompressionTask();
    void configure(int level = 3); // niveau de compression
    std::vector<uint8_t> run(const std::vector<uint8_t>& input);
    void log(const std::string& message) const;
private:
    int level_;
    bool configured_;
};
