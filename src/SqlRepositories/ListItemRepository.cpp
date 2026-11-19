#include "ListItemRepository.h"

ListItemRepository::ListItemRepository(PriorityService& priorityService, StatusService& statusService)
  : priorityService(priorityService)
  , statusService(statusService)
{
}

std::vector<ListItemEntity>
ListItemRepository::get(ListName& listName)
{
    std::vector<ListItemEntity> listItems;

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

    throw ListItemNotFoundException("Item not found: invalid id.", id, listName);
}

void
ListItemRepository::create(const ListItemEntity& item, ListName& listName)
{
    // fileDataService->append({ ListItemEntity::makeVector(item) });
}

bool
ListItemRepository::update(const std::string& id, ListName& listName, const ListItemEntity& item)
{
    std::vector<std::vector<std::string>> items;

    bool found = false;

    return found;
}

bool
ListItemRepository::remove(const std::string& id, ListName& listName)
{
    std::vector<std::vector<std::string>> items;

    bool found = false;

    return found;
}
