#include "ListRepository.h"

const std::string ListRepository::BACKUP_PREFIX = "._backup_del_";

ListRepository::ListRepository(ConfigService& configService, DataSerializerInterface* fileDataService)
  : configService(configService)
  , fileDataService(fileDataService)
  , fileStorageService(configService)
{
    path = configService.getListofListFilePath();
}

std::vector<ListEntity>
ListRepository::get()
{
    if (!cacheItems.empty()) {
        return cacheItems;
    }

    fileDataService->load(path);
    std::vector<std::vector<std::string>> data = fileDataService->read(std::nullopt);

    for (const std::vector<std::string>& item : data) {
        cacheItems.push_back(ListEntity::setFromVector(item));
    }

    return cacheItems;
}

ListEntity
ListRepository::find(const std::string& name)
{
    fileDataService->load(path);

    std::vector<std::vector<std::string>> data = fileDataService->read(0);

    for (const std::vector<std::string>& item : data) {
        if (item[0] == name) {
            return ListEntity::setFromVector(item);
        }
    }

    throw std::invalid_argument("List: " + name + " was not found.");
}

void
ListRepository::create(const ListEntity& item)
{
    fileDataService->load(path);
    fileDataService->append({ ListEntity::makeVector(item) });
    resetCache();
    fileStorageService.createNewListFile(*item.getName());
}

bool
ListRepository::update(const std::string& name, const ListEntity& item)
{
    fileDataService->load(path);

    std::vector<std::vector<std::string>> data = fileDataService->read(0);
    std::vector<std::vector<std::string>> items;

    bool found = false;
    for (std::vector itemFromStorage : data) {
        if (itemFromStorage[0] == name) {
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

void
ListRepository::rename(const std::string& oldName, const std::string& newName)
{
    ListEntity listEntity = find(oldName);
    // Moving the files first: it fails if the destination already exists, leaving everything untouched.
    fileStorageService.moveFileTo(oldName, newName);
    listEntity.setName(newName);
    update(oldName, listEntity);
}

void
ListRepository::remove(const std::string& name)
{
    removeEntry(name);
    fileStorageService.removeListFile(name);
}

void
ListRepository::removeKeepingBackup(const std::string& name)
{
    removeEntry(name);
    fileStorageService.moveFileTo(name, BACKUP_PREFIX + name);
}

void
ListRepository::removeEntry(const std::string& name)
{
    fileDataService->load(path);

    std::vector<std::vector<std::string>> data = fileDataService->read(0);
    std::vector<std::vector<std::string>> items;

    bool found = false;
    for (std::vector item : data) {
        if (item[0] == name) {
            found = true;
            continue;
        }
        items.push_back(item);
    }

    if (!found) {
        throw std::invalid_argument("List with name: " + name + " was not found.");
    }

    fileDataService->write(items);
    resetCache();
}

void
ListRepository::resetCache()
{
    cacheItems = {};
}
