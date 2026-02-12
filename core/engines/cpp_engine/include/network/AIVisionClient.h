#pragma once

#include <string>
#include <glm/glm.hpp>
#include <nlohmann/json.hpp>

namespace engine {

using json = nlohmann::json;

/**
 * TCP Client pour envoyer la vision/position de l'IA
 * La caméra du EngineRenderer suit les yeux de l'IA
 */
class AIVisionClient {
public:
    AIVisionClient(const std::string& host = "127.0.0.1", int port = 9999);
    ~AIVisionClient();
    
    // Connexion au serveur EngineRenderer
    bool connect();
    bool isConnected() const;
    
    // Envoyer la position et orientation de l'IA
    void updateVision(
        const glm::vec3& position,
        const glm::vec3& direction,  // direction de regard
        const glm::vec3& up = glm::vec3(0, 1, 0)
    );
    
    // Envoyer les données brutes
    void sendVisionData(const json& data);
    
    void disconnect();
    
private:
    std::string host_;
    int port_;
    int socket_;
    bool connected_;
    
    bool createSocket();
    bool sendData(const std::string& data);
};

} // namespace engine
