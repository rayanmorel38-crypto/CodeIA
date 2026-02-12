#ifndef CORE_H
#define CORE_H

#include <string>
#include <vector>

namespace core {

class Core {
public:
    Core();
    ~Core();

    void initialize();
    void shutdown();

    void addModule(const std::string& name);
    void removeModule(const std::string& name);

    std::vector<std::string> listModules() const;

private:
    std::vector<std::string> modules_;
    bool initialized_;
};

} // namespace core

#endif

