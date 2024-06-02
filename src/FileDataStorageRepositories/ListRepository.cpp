#include "ListRepository.h"

ListRepository::ListRepository(ConfigService& configService, FileDataServiceInterface* fileDataService)
    : configService(configService), fileDataService(fileDataService)
{
    path = configService.getListofListFilePath();
}

std::vector <ListEntity> ListRepository::get()
{
    if (!cacheItems.empty())
    {
        return cacheItems;
    }

    fileDataService->load(path);
    std::vector <std::vector <std::string>> data = fileDataService->read(std::nullopt);

    for (const std::vector <std::string>& item : data)
    {
        cacheItems.push_back(ListEntity::setFromVector(item));
    }

    return cacheItems;
}

ListEntity ListRepository::find(const std::string& key)
{
    fileDataService->load(path);

    std::vector < std::vector <std::string>> data = fileDataService->read(0);

    std::vector <ListEntity> items;

    for (std::vector item : data)
    {
        if (item[0] == key)
        {
            return ListEntity::setFromVector(item);
        }
    }

    throw std::invalid_argument("Config with key: " + key + " was not found.");
}

void ListRepository::create(const ListEntity& item)
{
    fileDataService->load(path);
    fileDataService->append({ ListEntity::makeVector(item)});
    resetCache();
}

bool ListRepository::update(const std::string& listName, const ListEntity& item)
{
    fileDataService->load(path);

    std::vector < std::vector <std::string>> data = fileDataService->read(0);
    std::vector < std::vector <std::string>> items;

    bool found = false;
    for (std::vector itemFromStorage : data)
    {
        if (itemFromStorage[0] == listName)
        {
            found = true;
            items.push_back(ListEntity::makeVector(item));
            continue;
        }
        items.push_back(itemFromStorage);
    }

    fileDataService->write(items);
    resetCache();
    return found;

}

void ListRepository::remove(std::string& name)
{
    fileDataService->load(path);

    std::vector < std::vector <std::string>> data = fileDataService->read(0);
    std::vector < std::vector <std::string>> items;

    bool found = false;
    for (std::vector item : data)
    {
        if (item[0] == name)
        {
            found = true;
            continue;
        }
        items.push_back(item);
    }

    if (!found)
    {
        throw std::invalid_argument("List with name: " + name + " was not found.");
    }

    fileDataService->write(items);
    resetCache();
}

void ListRepository::resetCache()
{
    cacheItems = {};
}
