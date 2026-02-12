// kinect_interface.cpp -- basic Kinect capture stub. Real backend optional (libfreenect2/OpenNI)

#include "modules/vision/kinect_interface.h"
#include <thread>
#include <chrono>
#include <cstring>
#include <iostream>

#ifdef WITH_FREENECT2
#include <libfreenect2/libfreenect2.hpp>
#include <libfreenect2/frame_listener_impl.h>
#include <libfreenect2/registration.h>
#endif

namespace cpp_engine::modules::vision {

KinectInterface::KinectInterface() {}
KinectInterface::~KinectInterface() { stop(); }

bool KinectInterface::initialize() {
#ifdef WITH_FREENECT2
    try {
        // Basic test of freenect2 availability; real device open on start
        libfreenect2::Freenect2 fn;
        if (fn.enumerateDevices() == 0) {
            std::cerr << "libfreenect2: no devices found" << std::endl;
            // still return true to allow stub behavior if needed
            return true;
        }
        return true;
    } catch (...) {
        std::cerr << "libfreenect2: initialization failed" << std::endl;
        return false;
    }
#else
    // No real backend available; we keep stub behavior
    return true;
#endif
}

bool KinectInterface::start(FrameCallback cb) {
    if (running_) return false;
    running_ = true;

#ifdef WITH_FREENECT2
    // Real backend path: open device and start listener
    std::thread([this, cb]() {
        libfreenect2::Freenect2 fn;
        std::string serial = fn.getDefaultDeviceSerialNumber();
        libfreenect2::Freenect2Device *dev = fn.openDevice(serial);
        if (!dev) {
            std::cerr << "Failed to open libfreenect2 device" << std::endl;
            running_ = false;
            return;
        }

        libfreenect2::SyncMultiFrameListener listener(libfreenect2::Frame::Color | libfreenect2::Frame::Depth);
        dev->setColorFrameListener(&listener);
        dev->setIrAndDepthFrameListener(&listener);
        dev->start();

        libfreenect2::Registration* registration = nullptr;
        try {
            libfreenect2::Freenect2Device::Config config;
            registration = new libfreenect2::Registration(dev->getIrCameraParams(), dev->getColorCameraParams());
        } catch (...) {
            registration = nullptr;
        }

        while (running_) {
            libfreenect2::FrameMap frames;
            if (!listener.waitForNewFrame(frames, 1000)) continue;
            libfreenect2::Frame *rgb = frames[libfreenect2::Frame::Color];
            libfreenect2::Frame *depth = frames[libfreenect2::Frame::Depth];

            KinectFrame frame;
            frame.width = rgb->width;
            frame.height = rgb->height;
            frame.rgb.assign(rgb->data, rgb->data + rgb->width * rgb->height * 4); // RGBA
            // convert depth
            frame.depth.resize(depth->width * depth->height);
            for (size_t i = 0; i < depth->width * depth->height; ++i) {
                frame.depth[i] = static_cast<uint16_t>(depth->data[i]);
            }

            // invoke callback
            cb(frame);

            listener.release(frames);
        }

        dev->stop();
        dev->close();
        if (registration) delete registration;
    }).detach();
    return true;
#else
    // Launch a simple thread that produces demo frames when no backend available
    std::thread([this, cb]() {
        int w = 640, h = 480;
        KinectFrame frame;
        frame.width = w;
        frame.height = h;
        frame.rgb.resize(w * h * 3);
        frame.depth.resize(w * h);

        while (running_) {
            for (int y = 0; y < h; ++y) {
                for (int x = 0; x < w; ++x) {
                    int i = (y * w + x) * 3;
                    frame.rgb[i+0] = (unsigned char)((x + (std::time(nullptr) % 255)) & 0xFF);
                    frame.rgb[i+1] = (unsigned char)((y + (std::time(nullptr) % 255)) & 0xFF);
                    frame.rgb[i+2] = 0;
                    frame.depth[y * w + x] = 0;
                }
            }
            cb(frame);
            std::this_thread::sleep_for(std::chrono::milliseconds(33)); // ~30fps
        }
    }).detach();

    return true;
#endif
}

void KinectInterface::stop() {
    running_ = false;
}

bool KinectInterface::is_running() const { return running_; }

} // namespace
