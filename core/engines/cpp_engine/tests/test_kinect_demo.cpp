// test_kinect_demo.cpp
#include "modules/vision/kinect_interface.h"
#include <iostream>
#include <atomic>
#include <chrono>
#include <thread>

int main() {
    using namespace cpp_engine::modules::vision;
    KinectInterface k;
    if (!k.initialize()) {
        std::cerr << "Kinect backend not available (stub)\n";
    }

    std::atomic<int> frames{0};

    bool ok = k.start([&frames](const KinectFrame &f){
        ++frames;
        if (frames % 30 == 0) {
            std::cout << "Received frame " << frames << " size=" << f.rgb.size() << "\n";
        }
    });

    if (!ok) {
        std::cerr << "Failed to start Kinect capture\n";
        return 1;
    }

    // Run for a few seconds
    std::this_thread::sleep_for(std::chrono::seconds(3));
    k.stop();
    std::cout << "Total frames: " << frames.load() << "\n";
    return 0;
}
