// kinect_test.cpp - Simple test program for Kinect integration
#include "modules/vision/kinect_interface.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>

int main() {
    std::cout << "Testing Kinect Interface..." << std::endl;

    cpp_engine::modules::vision::KinectInterface kinect;

    if (!kinect.initialize()) {
        std::cerr << "Failed to initialize Kinect interface" << std::endl;
        return 1;
    }

    std::cout << "Kinect interface initialized successfully" << std::endl;

    std::atomic<bool> running{true};

    // Start capture in a separate thread
    std::thread capture_thread([&]() {
        auto callback = [&](const cpp_engine::modules::vision::KinectFrame& frame) {
            std::cout << "Received frame: " << frame.width << "x" << frame.height
                      << ", RGB size: " << frame.rgb.size()
                      << ", Depth size: " << frame.depth.size() << std::endl;
        };

        if (kinect.start(callback)) {
            std::cout << "Kinect capture started" << std::endl;
            while (running) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
            kinect.stop();
            std::cout << "Kinect capture stopped" << std::endl;
        } else {
            std::cerr << "Failed to start Kinect capture" << std::endl;
        }
    });

    // Run for 5 seconds
    std::this_thread::sleep_for(std::chrono::seconds(5));
    running = false;

    capture_thread.join();

    std::cout << "Kinect test completed" << std::endl;
    return 0;
}