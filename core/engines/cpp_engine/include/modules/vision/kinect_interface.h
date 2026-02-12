// kinect_interface.h
#pragma once

#include <string>
#include <vector>
#include <functional>
#include <cstdint>

namespace cpp_engine::modules::vision {

struct KinectFrame {
    std::vector<unsigned char> rgb; // RGB bytes
    std::vector<uint16_t> depth;   // depth in millimeters
    int width;
    int height;
};

class KinectInterface {
public:
    using FrameCallback = std::function<void(const KinectFrame&)>;

    KinectInterface();
    ~KinectInterface();

    // Initialize Kinect backend (freenect2/OpenNI). Returns true if a backend is available.
    bool initialize();

    // Start capture thread, calling callback on each frame
    bool start(FrameCallback cb);
    void stop();

    bool is_running() const;

private:
    bool running_ = false;
};

} // namespace
