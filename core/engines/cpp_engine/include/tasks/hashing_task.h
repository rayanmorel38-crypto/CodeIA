#pragma once
/**
 * @class HashingTask
 * @brief Fonctions de hachage/cryptographie optimisées.
 *
 * Implémentation réelle: utiliser des bibliothèques crypto accélérées.
 */
#include <string>
#include <vector>
#include <stdexcept>

class HashingTask {
public:
    HashingTask();
    ~HashingTask();
    void configure(const std::string& algorithm = "sha256");
    std::vector<uint8_t> run(const std::vector<uint8_t>& input);
    void log(const std::string& message) const;
private:
    std::string algorithm_;
    bool configured_;
};
