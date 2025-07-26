#include "ListItemRepository.h"

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

std::vector<ListItemEntity>
ListItemRepository::get(ListName& listName)
{
    if (!cacheItems.empty()) {
        return cacheItems;
    }

    fileDataService->load(getFilePath(listName));

    std::vector<std::vector<std::string>> data = fileDataService->read(0);

    std::vector<ListItemEntity> listItems;

    if (data.empty()) {
        return listItems;
    }

    for (const std::vector<std::string>& item : data) {
        try {
            listItems.push_back(ListItemEntity::setFromVector(priorityService, statusService, item, listName));
        } catch (std::invalid_argument& e) {
            std::string invalidValue = StringHelpers::colorize("Invalid item -- please check storage", RED);
            std::vector<std::string> invalidItem = { "xxxx", invalidValue, "0", "0", "0", "0", "0", "0" };
            listItems.push_back(ListItemEntity::setFromVector(priorityService, statusService, invalidItem, listName));
        }
    }

    return listItems;
}

ListItemEntity
ListItemRepository::find(const std::string& id, ListName& listName)
{
    fileDataService->load(getFilePath(listName));

    std::vector<std::vector<std::string>> data = fileDataService->read(0);
    std::vector<std::vector<std::string>> items;

    int i = 0;
    for (std::vector item : data) {
        if (item[0] == id) {
            return ListItemEntity::setFromVector(priorityService, statusService, item, listName);
        }
        i++;
    }

    throw std::invalid_argument("Item with id: " + id + " was not found.");
}

void
ListItemRepository::create(const ListItemEntity& item, ListName& listName)
{
    fileDataService->load(getFilePath(listName));
    fileDataService->append({ ListItemEntity::makeVector(item) });
    resetCache();
}

bool
ListItemRepository::update(const std::string& id, ListName& listName, const ListItemEntity& item)
{
    fileDataService->load(getFilePath(listName));

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
ListItemRepository::remove(const std::string& id, ListName& listName)
{
    fileDataService->load(getFilePath(listName));

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
