#ifndef CONFIGENTITY_H
#define CONFIGENTITY_H

#include <string>
#include <map>
#include <vector>

class ConfigEntity {
public:
    std::string getKey() const;
    std::string getValue() const;
    void setKey(std::string key);
    void setValue(std::string value);
    void set(std::string key, std::string value);
    static ConfigEntity setFromVector(std::vector<std::string> item);
    static std::vector <std::string> makeVector(const ConfigEntity& configEntity);

private:
    std::string key;
    std::string value;
    std::vector <std::string> arguments;
    std::map <std::string, std::string> config;
};

#endif //CONFIGENTITY_H
