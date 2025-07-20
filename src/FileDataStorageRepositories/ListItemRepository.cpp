#include "ListItemRepository.h"

#include <filesystem>
#include <fstream>
#include <utility>
#include <vector>

ListItemRepository::ListItemRepository(ConfigService& configService,
                                       FileDataServiceInterface* fileDataService,
                                       PriorityService& priorityService,
                                       StatusService& statusService)
  : configService(configService)
  , fileDataService(fileDataService)
  , priorityService(priorityService)
  , statusService(statusService)
{
}

ListItemRepository&
ListItemRepository::load(const std::string& listNameStr, std::string variantStr)
{
    std::filesystem::path path;
    if (listNameStr.empty()) {
        this->listName = configService.getCurrentList();
        path = configService.getCurrentListFilePath();
    } else {
        this->listName = listNameStr;
        path = configService.getListFilePath(listNameStr);
    }
    //    this->listVariant = variantStr;

    if (variantStr == "archive") {
        path = configService.getListArchiveFilePathFromFilePath(path);
    } else if (variantStr == "delete") {
        path = configService.getListDeleteFilePathFromFilePath(path);
    }

    if (path == listFilePath) {
        return *this;
    }

    this->listFilePath = path;
    return *this;
}

ListItemRepository&
ListItemRepository::loadVariant(const std::string& variantStr)
{
    std::filesystem::path path;

    if (variantStr == "archive") {
        path = configService.getListArchiveFilePathFromFilePath(path);
    } else if (variantStr == "delete") {
        path = configService.getListDeleteFilePathFromFilePath(path);
    }

    if (path == listFilePath) {
        return *this;
    }

    this->listFilePath = path;
    return *this;
}

std::vector<ListItemEntity>
ListItemRepository::get()
{
    if (!cacheItems.empty()) {
        return cacheItems;
    }

    fileDataService->load(listFilePath);

    std::vector<std::vector<std::string>> data = fileDataService->read(0);

    std::vector<ListItemEntity> listItems;

    if (data.empty()) {
        return listItems;
    }

    for (const std::vector<std::string>& item : data) {
        try {
            listItems.push_back(
                ListItemEntity::setFromVector(priorityService, statusService, item, listName, listVariant));
        } catch (std::invalid_argument& e) {
            std::string invalidValue = StringHelpers::colorize("Invalid item -- please check storage", RED);
            std::vector<std::string> invalidItem = { "xxxx", invalidValue, "0", "0", "0", "0", "0", "0" };
            listItems.push_back(
                ListItemEntity::setFromVector(priorityService, statusService, invalidItem, listName, listVariant));
        }
    }

    return listItems;
}

ListItemEntity
ListItemRepository::find(const std::string& id)
{
    fileDataService->load(listFilePath);

    std::vector<std::vector<std::string>> data = fileDataService->read(0);
    std::vector<std::vector<std::string>> items;

    int i = 0;
    for (std::vector item : data) {
        if (item[0] == id) {
            return ListItemEntity::setFromVector(priorityService, statusService, item, listName, listVariant);
        }
        i++;
    }

    throw std::invalid_argument("Item with id: " + id + " was not found.");
}

void
ListItemRepository::create(const ListItemEntity& item)
{
    fileDataService->load(listFilePath);
    fileDataService->append({ ListItemEntity::makeVector(item) });
    resetCache();
}

bool
ListItemRepository::update(const std::string& id, const ListItemEntity& item)
{
    fileDataService->load(listFilePath);

    std::vector<std::vector<std::string>> data = fileDataService->read(0);
    std::vector<std::vector<std::string>> items;

    bool found = false;
    for (std::vector itemFromStorage : data) {
        if (itemFromStorage[0] == id) {
            found = true;
            items.push_back(ListItemEntity::makeVector(item));
            continue;
        }
        items.push_back(itemFromStorage);
    }

    fileDataService->write(items);
    resetCache();
    return found;
}

bool
ListItemRepository::remove(const std::string& id)
{
    fileDataService->load(listFilePath);

    std::vector<std::vector<std::string>> data = fileDataService->read(0);
    std::vector<std::vector<std::string>> items;

    bool found = false;
    for (std::vector item : data) {
        if (item[0] == id) {
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
ListItemRepository::resetCache()
{
    cacheItems = {};
}
