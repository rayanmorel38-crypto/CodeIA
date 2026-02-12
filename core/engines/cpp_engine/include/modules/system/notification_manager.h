#ifndef CPP_ENGINE_MODULES_SYSTEM_NOTIFICATION_MANAGER_H
#define CPP_ENGINE_MODULES_SYSTEM_NOTIFICATION_MANAGER_H

#include <string>
#include <vector>
#include <chrono>

namespace cppengine { namespace modules { namespace system {

enum class NotificationPriority {
    LOW,
    MEDIUM,
    HIGH,
    CRITICAL
};

struct Notification {
    std::string id;
    std::string title;
    std::string message;
    NotificationPriority priority;
    std::chrono::system_clock::time_point timestamp;
};

class NotificationManager {
public:
    static NotificationManager& instance();
    void send_notification(const std::string& title, const std::string& message,
                          NotificationPriority priority = NotificationPriority::MEDIUM);
    std::vector<Notification> get_notifications(NotificationPriority min_priority = NotificationPriority::LOW);
    void clear_notifications();

private:
    NotificationManager();
    ~NotificationManager();
    
    std::vector<Notification> notifications_;
    size_t notification_count_;
};

} } }

#endif // CPP_ENGINE_MODULES_SYSTEM_NOTIFICATION_MANAGER_H