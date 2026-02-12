#include "modules/network/network_client.h"
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <queue>
#include <functional>
#include <iostream>
#include <sstream>
#include "utils/logger.h"

namespace cppengine { namespace modules { namespace network {

NetworkClient::NetworkClient() : running_(false) {}

NetworkClient::~NetworkClient() {
    shutdown();
}

NetworkClient& NetworkClient::instance() {
    static NetworkClient instance;
    return instance;
}

void NetworkClient::init() {
    if (running_) return;
    running_ = true;
    network_thread_ = std::thread(&NetworkClient::process_queue, this);
    cpp_engine::utils::Logger::instance().info("[NetworkClient] initialized");
}

void NetworkClient::shutdown() {
    if (!running_) return;
    running_ = false;
    cv_.notify_all();
    if (network_thread_.joinable()) {
        network_thread_.join();
    }
    cpp_engine::utils::Logger::instance().info("[NetworkClient] shutdown");
}

void NetworkClient::send_request(const std::string& endpoint, const std::string& method,
                                const std::string& payload,
                                std::function<void(const std::string&)> callback) {
    std::lock_guard<std::mutex> lock(mutex_);
    NetworkMessage msg{std::to_string(++request_count_), endpoint, method, payload, callback};
    request_queue_.push(msg);
    cv_.notify_one();
    cpp_engine::utils::Logger::instance().info("[NetworkClient] queued request: " + method + " " + endpoint);
}

std::string NetworkClient::send_sync_request(const std::string& endpoint, const std::string& method,
                                           const std::string& payload) {
    // Mock response for demo
    std::string response = "Mock response for " + method + " " + endpoint;
    if (!payload.empty()) {
        response += " with payload: " + payload;
    }
    
    cpp_engine::utils::Logger::instance().info("[NetworkClient] sync request: " + method + " " + endpoint);
    return response;
}

void NetworkClient::process_queue() {
    while (running_) {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this]() { return !running_ || !request_queue_.empty(); });
        
        if (!running_) break;
        
        NetworkMessage msg = request_queue_.front();
        request_queue_.pop();
        lock.unlock();
        
        // Mock network request processing
        std::string response = "Mock async response for " + msg.method + " " + msg.endpoint;
        
        if (msg.callback) {
            try {
                msg.callback(response);
            } catch (const std::exception& e) {
                cpp_engine::utils::Logger::instance().error("[NetworkClient] callback failed: " + std::string(e.what()));
            }
        }
        
        cpp_engine::utils::Logger::instance().info("[NetworkClient] processed request: " + msg.id);
    }
}

} } }