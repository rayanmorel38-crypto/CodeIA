#ifndef CPP_ENGINE_MODULES_NETWORK_NETWORK_CLIENT_H
#define CPP_ENGINE_MODULES_NETWORK_NETWORK_CLIENT_H

#include <string>
#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

namespace cppengine { namespace modules { namespace network {

class NetworkClient {
public:
    static NetworkClient& instance();
    void init();
    void shutdown();
    void send_request(const std::string& endpoint, const std::string& method,
                     const std::string& payload = "",
                     std::function<void(const std::string&)> callback = nullptr);
    std::string send_sync_request(const std::string& endpoint, const std::string& method,
                                 const std::string& payload = "");

private:
    NetworkClient();
    ~NetworkClient();
    
    void process_queue();
    
    struct NetworkMessage {
        std::string id;
        std::string endpoint;
        std::string method;
        std::string payload;
        std::function<void(const std::string&)> callback;
    };
    
    std::thread network_thread_;
    std::mutex mutex_;
    std::condition_variable cv_;
    std::queue<NetworkMessage> request_queue_;
    bool running_;
    size_t request_count_;
};

} } }

#endif // CPP_ENGINE_MODULES_NETWORK_NETWORK_CLIENT_H