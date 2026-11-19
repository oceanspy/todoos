#include "ListRepository.h"

ListRepository::ListRepository() {}

std::vector<ListEntity>
ListRepository::get()
{
    std::vector<ListEntity> items;

    return items;
}

ListEntity
ListRepository::find(const std::string& key)
{
    ListEntity item;

    return item;

    throw std::invalid_argument("List: " + key + " was not found.");
}

void
ListRepository::create(const ListEntity& item)
{
    // fileDataService->append({ ListEntity::makeVector(item) });
}

bool
ListRepository::update(const std::string& listName, const ListEntity& item)
{
    bool found = false;
    return found;
}

void
ListRepository::remove(std::string& name)
{
}
