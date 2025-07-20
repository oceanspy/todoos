#include "ConfigRepository.h"

#include <utility>

ConfigRepository::ConfigRepository(FileDataServiceInterface* fileDataService, const std::filesystem::path filePath)
  : fileDataService(fileDataService)
  , filePath(filePath)
{
}

std::vector<ConfigEntity>
ConfigRepository::get()
{
    if (!cacheItems.empty()) {
        return cacheItems;
    }

    fileDataService->load(filePath);
    std::vector<std::vector<std::string>> data = fileDataService->read(0);

    for (std::vector<std::string> item : data) {
        cacheItems.push_back(ConfigEntity::setFromVector(item));
    }

    return cacheItems;
}

ConfigEntity
ConfigRepository::find(const std::string& key)
{
    fileDataService->load(filePath);

    std::vector<std::vector<std::string>> data = fileDataService->read(0);

    for (std::vector item : data) {
        if (item[0] == key) {
            return ConfigEntity::setFromVector(item);
        }
    }

    throw std::invalid_argument("Config with key: " + key + " was not found.");
}

void
ConfigRepository::create(ConfigEntity item)
{
    fileDataService->load(filePath);
    fileDataService->append({ ConfigEntity::makeVector(item) });
    resetCache();
}

bool
ConfigRepository::update(const std::string& key, const ConfigEntity& item)
{
    fileDataService->load(filePath);

    std::vector<std::vector<std::string>> data = fileDataService->read(0);
    std::vector<std::vector<std::string>> items;

    bool found = false;
    for (std::vector itemFromStorage : data) {
        if (itemFromStorage[0] == key) {
            found = true;
            items.push_back(ConfigEntity::makeVector(item));
            continue;
        }
        items.push_back(itemFromStorage);
    }

    fileDataService->write(items);
    resetCache();
    return found;
}

bool
ConfigRepository::remove(std::string& key)
{
    fileDataService->load(filePath);

    std::vector<std::vector<std::string>> data = fileDataService->read(0);

    std::vector<std::vector<std::string>> items;

    bool found = false;
    for (std::vector item : data) {
        if (item[0] == key) {
            found = true;
            continue;
        }
        items.push_back(item);
    }

    fileDataService->write(items);
    resetCache();
    return found;
}

void
ConfigRepository::resetCache()
{
    cacheItems = {};
}
