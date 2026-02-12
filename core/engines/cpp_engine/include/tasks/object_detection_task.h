#pragma once
/**
 * @class ObjectDetectionTask
 * @brief Tâche de détection d'objets pour le moteur IA.
 *
 * Permet de détecter et localiser des objets dans une image ou une vidéo.
 */
#include <string>
#include <vector>
#include <stdexcept>

struct DetectionResult {
    std::string label;
    float confidence;
    int x, y, width, height;
};

class ObjectDetectionTask {
public:
    /**
     * @brief Constructeur par défaut
     */
    ObjectDetectionTask();
    /**
     * @brief Destructeur
     */
    ~ObjectDetectionTask();
    /**
     * @brief Configure la tâche avec un modèle et des paramètres
     * @param modelPath Chemin vers le modèle
     * @param threshold Seuil de confiance
     */
    void configure(const std::string& modelPath, float threshold = 0.5f);
    /**
     * @brief Exécute la détection sur une image
     * @param imagePath Chemin de l'image à analyser
     * @return Liste des résultats de détection
     */
    std::vector<DetectionResult> run(const std::string& imagePath);
    /**
     * @brief Affiche les logs de la tâche
     */
    void log(const std::string& message) const;
private:
    std::string modelPath_;
    float threshold_;
    bool configured_;
};
