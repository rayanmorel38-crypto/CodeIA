#pragma once
/**
 * @class AudioGenerationTask
 * @brief Génération audio (TTS / synthèse) - interface robuste.
 *
 * Fournit une API simple pour configurer un moteur de génération audio,
 * lancer une génération à partir d'un prompt/paramètres et sauvegarder
 * le résultat en fichier WAV. L'implémentation réelle peut déléguer à
 * des backends (e.g. TTS local, neural vocoder, etc.).
 */
#include <string>
#include <vector>
#include <stdexcept>

class AudioGenerationTask {
public:
    AudioGenerationTask();
    ~AudioGenerationTask();

    // Configure le moteur de génération audio.
    // modelPath peut être vide (backend par défaut)
    void configure(const std::string& modelPath = "", int sampleRate = 48000, int channels = 1);

    // Génère de l'audio à partir d'un prompt (texte) ou d'un identifiant de job.
    // Retourne un buffer PCM float normalized [-1,1].
    std::vector<float> run(const std::string& prompt);

    // Sauvegarde un buffer PCM float en WAV 16-bit
    static bool saveToWav(const std::string& outPath, const std::vector<float>& pcm, int sampleRate, int channels);

    void log(const std::string& message) const;

private:
    std::string modelPath_;
    int sampleRate_;
    int channels_;
    bool configured_;
};
