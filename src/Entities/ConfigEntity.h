#ifndef CONFIGENTITY_H
#define CONFIGENTITY_H

#include <string>
#include <map>
#include <vector>

class ConfigEntity {
public:
    [[nodiscard]] const std::string * getKey() const;
    [[nodiscard]] const std::string * getValue() const;
    void setKey(const std::string& key);
    void setValue(const std::string& value);
    void set(const std::string& key, const std::string& value);
    static ConfigEntity setFromVector(const std::vector<std::string>& item);
    static std::vector <std::string> makeVector(const ConfigEntity& configEntity);

private:
    std::string key;
    std::string value;
    std::vector <std::string> arguments;
    std::map <std::string, std::string> config;
};

#endif //CONFIGENTITY_H
