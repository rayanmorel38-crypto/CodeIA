#ifndef CPP_ENGINE_HTTP_SERVER_H
#define CPP_ENGINE_HTTP_SERVER_H

#include <string>
#include <functional>
#include <memory>
#include <thread>
#include <map>
#include <atomic>

namespace cppengine {
namespace network {

/**
 * HTTP Server for native C++ API
 * Provides endpoints for process control, task tracking, and result retrieval
 */
class HttpServer {
public:
    struct Config {
        std::string host = "127.0.0.1";
        int port = 3004;
        int num_threads = 4;
        std::string cpp_bin;
        int default_timeout_seconds = 60;
        int retention_seconds = 3600;
    };

    /**
     * Initialize HTTP server with configuration
     */
    HttpServer();
    explicit HttpServer(const Config& config);
    
    ~HttpServer();

    /**
     * Start the HTTP server (blocking)
     */
    void start();

    /**
     * Stop the HTTP server
     */
    void stop();

    /**
     * Check if server is running
     */
    bool is_running() const;

    /**
     * Register a custom endpoint handler
     * @param method HTTP method (GET, POST, etc)
     * @param path URL path
     * @param handler Function that processes the request
     */
    void on(const std::string& method, const std::string& path,
            std::function<void(const std::string&, std::string&)> handler);

private:
    Config config_;
    std::atomic<bool> running_{false};
    std::atomic<bool> cleanup_running_{false};
    std::thread cleanup_thread_;
};

}  // namespace network
}  // namespace cppengine

#endif // CPP_ENGINE_HTTP_SERVER_H
