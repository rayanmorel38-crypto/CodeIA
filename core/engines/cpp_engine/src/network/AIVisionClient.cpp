#include "network/AIVisionClient.h"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

namespace engine {

AIVisionClient::AIVisionClient(const std::string& host, int port)
    : host_(host), port_(port), socket_(-1), connected_(false) {
}

AIVisionClient::~AIVisionClient() {
    disconnect();
}

bool AIVisionClient::connect() {
    if (!createSocket()) {
        std::cerr << "❌ Impossible de créer le socket\n";
        return false;
    }
    
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port_);
    inet_pton(AF_INET, host_.c_str(), &serverAddr.sin_addr);
    
    if (::connect(socket_, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "❌ Connexion échouée à " << host_ << ":" << port_ << "\n";
        close(socket_);
        socket_ = -1;
        return false;
    }
    
    connected_ = true;
    std::cout << "✅ Connecté au EngineRenderer: " << host_ << ":" << port_ << "\n";
    return true;
}

bool AIVisionClient::isConnected() const {
    return connected_ && socket_ >= 0;
}

bool AIVisionClient::createSocket() {
    socket_ = ::socket(AF_INET, SOCK_STREAM, 0);
    return socket_ >= 0;
}

bool AIVisionClient::sendData(const std::string& data) {
    if (!isConnected()) {
        return false;
    }
    
    size_t totalSent = 0;
    while (totalSent < data.size()) {
        ssize_t sent = ::send(socket_, 
                             data.c_str() + totalSent,
                             data.size() - totalSent,
                             0);
        if (sent <= 0) {
            connected_ = false;
            return false;
        }
        totalSent += sent;
    }
    
    return true;
}

void AIVisionClient::updateVision(
    const glm::vec3& position,
    const glm::vec3& direction,
    const glm::vec3& up) {
    
    json visionData;
    visionData["type"] = "ai_vision";
    visionData["position"] = {
        {"x", position.x},
        {"y", position.y},
        {"z", position.z}
    };
    visionData["direction"] = {
        {"x", direction.x},
        {"y", direction.y},
        {"z", direction.z}
    };
    visionData["up"] = {
        {"x", up.x},
        {"y", up.y},
        {"z", up.z}
    };
    
    sendVisionData(visionData);
}

void AIVisionClient::sendVisionData(const json& data) {
    std::string jsonStr = data.dump();
    jsonStr += "\n";  // Newline comme délimiteur
    
    if (!sendData(jsonStr)) {
        std::cerr << "⚠️  Erreur d'envoi des données de vision\n";
        if (!connect()) {
            std::cerr << "⚠️  Reconnexion échouée\n";
        }
    }
}

void AIVisionClient::disconnect() {
    if (socket_ >= 0) {
        close(socket_);
        socket_ = -1;
    }
    connected_ = false;
    std::cout << "🛑 Vision client déconnecté\n";
}

} // namespace engine
