#include "SqlListRepository.h"
#include <stdexcept>

SqlListRepository::SqlListRepository() {}

std::vector<ListEntity>
SqlListRepository::get()
{
    std::vector<ListEntity> items;

    return items;
}

ListEntity
SqlListRepository::find(const std::string& name)
{
    ListEntity item;

    return item;

    throw std::invalid_argument("List: " + name + " was not found.");
}

void
SqlListRepository::create(const ListEntity& item)
{
    // fileDataService->append({ ListEntity::makeVector(item) });
}

bool
SqlListRepository::update(const std::string& name, const ListEntity& item)
{
    bool found = false;
    return found;
}

void
SqlListRepository::rename(const std::string& oldName, const std::string& newName)
{
}

void
SqlListRepository::remove(const std::string& name)
{
}

void
SqlListRepository::removeKeepingBackup(const std::string& name)
{
}
