#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <map>

namespace config {

class Config {
public:
    Config();
    ~Config();

    bool loadFromFile(const std::string& path);
    bool saveToFile(const std::string& path) const;

    std::string getValue(const std::string& key) const;
    void setValue(const std::string& key, const std::string& value);

private:
    std::map<std::string, std::string> data_;
};

} // namespace config

#endif

