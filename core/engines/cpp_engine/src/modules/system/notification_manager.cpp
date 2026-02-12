#include "modules/system/notification_manager.h"
#include <string>
#include <vector>
#include <iostream>
#include <chrono>
#include <thread>
#include "utils/logger.h"

namespace cppengine { namespace modules { namespace system {

NotificationManager::NotificationManager() : notification_count_(0) {}

NotificationManager::~NotificationManager() {}

NotificationManager& NotificationManager::instance() {
    static NotificationManager instance;
    return instance;
}

void NotificationManager::send_notification(const std::string& title, const std::string& message, NotificationPriority priority) {
    Notification notif{
        "notif_" + std::to_string(notification_count_++),
        title,
        message,
        priority,
        std::chrono::system_clock::now()
    };

    notifications_.push_back(notif);

    // Log the notification
    std::string priority_str;
    switch (priority) {
        case NotificationPriority::LOW: priority_str = "LOW"; break;
        case NotificationPriority::MEDIUM: priority_str = "MEDIUM"; break;
        case NotificationPriority::HIGH: priority_str = "HIGH"; break;
        case NotificationPriority::CRITICAL: priority_str = "CRITICAL"; break;
    }

    cpp_engine::utils::Logger::instance().info(
        "[Notification] " + priority_str + " - " + title + ": " + message
    );

    // In a real system, this would integrate with desktop notifications,
    // email, SMS, etc. For now, just print to console for demo
    if (priority >= NotificationPriority::HIGH) {
        std::cout << "\n🔔 " << title << ": " << message << std::endl;
    }
}

std::vector<Notification> NotificationManager::get_notifications(NotificationPriority min_priority) {
    std::vector<Notification> filtered;
    for (const auto& notif : notifications_) {
        if (notif.priority >= min_priority) {
            filtered.push_back(notif);
        }
    }
    return filtered;
}

void NotificationManager::clear_notifications() {
    notifications_.clear();
    cpp_engine::utils::Logger::instance().info("[NotificationManager] cleared all notifications");
}

} } }