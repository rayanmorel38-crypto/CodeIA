#include "network/http_server.h"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    std::string host = "127.0.0.1";
    int port = 3004;
    
    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--host" && i + 1 < argc) {
            host = argv[++i];
        } else if (arg == "--port" && i + 1 < argc) {
            port = std::stoi(argv[++i]);
        } else if (arg == "-h" || arg == "--help") {
            std::cout << "cpp_engine HTTP Server\n"
                      << "Usage: cpp_engine_server [OPTIONS]\n"
                      << "Options:\n"
                      << "  --host <HOST>  Bind to host (default: 127.0.0.1)\n"
                      << "  --port <PORT>  Bind to port (default: 3004)\n"
                      << "  -h, --help     Show this help message\n";
            return 0;
        }
    }
    
    try {
        cppengine::network::HttpServer::Config config;
        config.host = host;
        config.port = port;
        
        cppengine::network::HttpServer server(config);
        server.start();
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
}
