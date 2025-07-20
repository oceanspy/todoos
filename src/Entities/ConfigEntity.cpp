#include "ConfigEntity.h"
#include <stdexcept>

void
ConfigEntity::set(const std::string& key, const std::string& value)
{
    this->key = key;
    this->value = value;
}

void
ConfigEntity::setKey(const std::string& key)
{
    for (char ch : key) {
        if (!std::isalnum(ch) && ch != '_') {
            throw std::invalid_argument("KEY must be alphanumeric or _");
        }
    }

    this->key = key;
}

void
ConfigEntity::setValue(const std::string& value)
{
    for (char ch : key) {
        if (!std::isalnum(ch) && ch != '_') {
            throw std::invalid_argument("KEY must be alphanumeric or _");
        }
    }

    this->value = value;
}

const std::string*
ConfigEntity::getKey() const
{
    return &key;
}

const std::string*
ConfigEntity::getValue() const
{
    return &value;
}

ConfigEntity
ConfigEntity::setFromVector(const std::vector<std::string>& item)
{
    ConfigEntity configEntity;
    configEntity.set(item[0], item[1]);
    return configEntity;
}

std::vector<std::string>
ConfigEntity::makeVector(const ConfigEntity& configEntity)
{
    return { *configEntity.getKey(), *configEntity.getValue() };
}
