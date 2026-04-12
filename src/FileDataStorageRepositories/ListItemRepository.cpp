#include "ListItemRepository.h"

ListItemRepository::ListItemRepository(ConfigService& configService,
                                       DataSerializerInterface* fileDataService,
                                       PriorityService& priorityService,
                                       StatusService& statusService)
  : configService(configService)
  , fileDataService(fileDataService)
  , priorityService(priorityService)
  , statusService(statusService)
{
}

std::vector<ListItemEntity>
ListItemRepository::get(ListName& listName)
{
    std::string filePath = getFilePath(listName);

    auto it = cacheItems.find(filePath);
    if (it != cacheItems.end()) {
        return it->second;
    }

    fileDataService->load(filePath);

    std::vector<std::vector<std::string>> data = fileDataService->read(0);

    std::vector<ListItemEntity> listItems;

    if (!data.empty()) {
        for (const std::vector<std::string>& item : data) {
            try {
                listItems.push_back(ListItemEntity::setFromVector(priorityService, statusService, item, listName));
            } catch (std::invalid_argument& e) {
                std::string invalidValue = StringHelpers::colorize("Invalid item -- please check storage", RED);
                std::vector<std::string> invalidItem = { "xxxx", invalidValue, "0", "0", "0", "0", "0", "0" };
                listItems.push_back(ListItemEntity::setFromVector(priorityService, statusService, invalidItem, listName));
            }
        }
    }

    cacheItems[filePath] = listItems;
    return listItems;
}

ListItemEntity
ListItemRepository::find(const std::string& id, ListName& listName)
{
    for (const ListItemEntity& item : get(listName)) {
        if (*item.getId() == id) {
            return item;
        }
    }
    throw std::invalid_argument("Item with id: " + id + " was not found.");
}

void
ListItemRepository::create(const ListItemEntity& item, ListName& listName)
{
    std::string filePath = getFilePath(listName);
    fileDataService->load(filePath);
    fileDataService->append({ ListItemEntity::makeVector(item) });
    resetCache(filePath);
}

bool
ListItemRepository::update(const std::string& id, ListName& listName, const ListItemEntity& item)
{
    std::string filePath = getFilePath(listName);
    fileDataService->load(filePath);

    std::vector<std::vector<std::string>> data = fileDataService->read(0);
    std::vector<std::vector<std::string>> items;

    bool found = false;
    for (const std::vector<std::string>& itemFromStorage : data) {
        if (!itemFromStorage.empty() && itemFromStorage[0] == id) {
            found = true;
            items.push_back(ListItemEntity::makeVector(item));
            continue;
        }
        items.push_back(itemFromStorage);
    }

    fileDataService->write(items);
    resetCache(filePath);
    return found;
}

bool
ListItemRepository::remove(const std::string& id, ListName& listName)
{
    std::string filePath = getFilePath(listName);
    fileDataService->load(filePath);

    std::vector<std::vector<std::string>> data = fileDataService->read(0);
    std::vector<std::vector<std::string>> items;

    bool found = false;
    for (const std::vector<std::string>& item : data) {
        if (!item.empty() && item[0] == id) {
            found = true;
            continue;
        }
        items.push_back(item);
    }

    fileDataService->write(items);
    resetCache(filePath);
    return found;
}

void
ListItemRepository::resetCache(const std::string& filePath)
{
    if (filePath.empty()) {
        cacheItems.clear();
    } else {
        cacheItems.erase(filePath);
    }
}

std::string
ListItemRepository::getFilePath(ListName& listName)
{
    std::filesystem::path path = configService.getListFilePath(listName.getName());

    if (listName.getVariant() == "archive") {
        path = configService.getListArchiveFilePathFromFilePath(path);
    } else if (listName.getVariant() == "delete") {
        path = configService.getListDeleteFilePathFromFilePath(path);
    }

    return path;
}
