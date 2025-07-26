#include "ListItemService.h"
#include "ListName.h"

#include <string>
#include <utility>

ListItemService::ListItemService(IOService& ioService,
                                 ConfigService& configService,
                                 ListItemRepository& listItemRepository,
                                 PriorityService& priorityService,
                                 StatusService& statusService)
  : ioService(ioService)
  , configService(configService)
  , listItemRepository(listItemRepository)
  , priorityService(priorityService)
  , statusService(statusService)
{
}

std::vector<ListItemEntity>
ListItemService::get(ListName& listName)
{
    return sort(listItemRepository.get(listName));
}

ListItemEntity
ListItemService::find(const std::string& id, ListName& listName)
{
    return listItemRepository.find(id, listName);
}

bool
ListItemService::remove(const std::string& id, ListName& listName)
{
    return listItemRepository.remove(id, listName);
}

std::string
ListItemService::add(ListName& listName,
                     const std::string& itemValue,
                     const std::string* priority,
                     const std::string* status,
                     time_t dueAt)
{
    if (itemValue.length() > 255) {
        throw std::invalid_argument("Item value must not exceed 255 characters.");
    }

    std::string id = makeId(listName);

    ListItemEntity listItemEntity(listName);
    listItemEntity.setId(id);

    if (priority != nullptr) {
        PriorityEntity priorityEntity = priorityService.getPriorityFromName(*priority);
        listItemEntity.setPriority(priorityEntity);
    } else {
        PriorityEntity priorityEntity = priorityService.getPriorityFromName("medium");
        listItemEntity.setPriority(priorityEntity);
    }
    if (status != nullptr) {
        StatusEntity statusEntity = statusService.getStatusFromName(*status);
        listItemEntity.setStatus(statusEntity);
    } else {
        StatusEntity statusEntity = statusService.getStatusFromName("to-do");
        listItemEntity.setStatus(statusEntity);
    }

    listItemEntity.setValue(itemValue);
    listItemEntity.setCreatedAt(time(nullptr));
    listItemEntity.setUpdatedAt(time(nullptr));
    listItemEntity.setDueAt(dueAt);
    listItemRepository.create(listItemEntity, listName);

    return id;
}

std::string
ListItemService::makeId(ListName& listName)
{
    bool validId = false;
    std::string id;
    int i = 0;
    while (!validId && i < 50) {
        if (configService.getValue("idRandomGenerationType") == idLettersLowercase) {
            id = StringHelpers::randomLettersLowercase(idLength);
        } else if (configService.getValue("idRandomGenerationType") == idLetters) {
            id = StringHelpers::randomAlNumString(idLength);
        } else {
            id = StringHelpers::randomString(idLength);
        }

        if (isIdAvailable(id, listName)) {
            validId = true;
        }

        i++;
    }

    return id;
}

bool
ListItemService::isIdAvailable(const std::string& id, ListName& listName)
{
    ListName listNameArchive = ListName::createVariant(listName, "archive");
    ListName listNameDelete = ListName::createVariant(listName, "delete");

    // TODO: Optimize this
    try {
        find(id, listName);
        return false;
    } catch (std::exception& e) {
        // id is not in default list
    }

    try {
        find(id, listNameArchive);
        return false;
    } catch (std::exception& e) {
        // id is not in archive list
    }

    try {
        find(id, listNameDelete);
        return false;
    } catch (std::exception& e) {
        // id is not in delete list
    }

    return true;
}

void
ListItemService::edit(const std::string& id,
                      ListName& listName,
                      const std::string& itemValue,
                      const std::string* priority,
                      const std::string* status)
{
    if (itemValue.length() > 255) {
        throw std::invalid_argument("Item value must not exceed 255 characters.");
    }

    if (itemValue.empty() && priority == nullptr && status == nullptr) {
        throw std::invalid_argument("No values to update.");
    }

    ListItemEntity listItemToUpdate(listName);
    listItemToUpdate = find(id, listName);

    if (priority != nullptr) {
        PriorityEntity priorityEntity = priorityService.getPriorityFromName(*priority);
        listItemToUpdate.setPriority(priorityEntity);
    }
    if (status != nullptr) {
        StatusEntity statusEntity = statusService.getStatusFromName(*status);
        listItemToUpdate.setStatus(statusEntity);
    }

    if (!itemValue.empty()) {
        listItemToUpdate.setValue(itemValue);
    }
    listItemToUpdate.setUpdatedAt(time(nullptr));

    listItemRepository.update(id, listName, listItemToUpdate);
}

void
ListItemService::editStatus(const std::string& id, ListName& listName, const int* status)
{
    ListItemEntity listItemToUpdate(listName);
    listItemToUpdate = find(id, listName);
    // check if status is not already set
    if (*(*listItemToUpdate.status()).getId() == *status) {
        throw std::invalid_argument("Status of: " + id + " already set to: " + std::to_string(*status));
    }

    StatusEntity statusEntity = statusService.find(*status);
    listItemToUpdate.setStatus(statusEntity);
    listItemToUpdate.setUpdatedAt(time(nullptr));

    if (statusService.isClosed(statusService.getNameFromId(*status))) {
        listItemToUpdate.setClosedAt(time(nullptr));
    } else {
        listItemToUpdate.setClosedAt(0);
    }

    listItemRepository.update(id, listName, listItemToUpdate);

    if (configService.isTrue("archiveWhenCompleted")) {
        if (statusService.isClosed(statusService.getNameFromId(*status))) {
            archive(id, listName);
        }
    }
}

void
ListItemService::reset(const std::string& id, ListName& listName)
{
    ListItemEntity listItemToUpdate(listName);
    listItemToUpdate = find(id, listName);
    StatusEntity statusEntity = statusService.getStatusFromName("to-do");
    listItemToUpdate.setStatus(statusEntity);
    listItemToUpdate.setCreatedAt(time(nullptr));
    listItemToUpdate.setUpdatedAt(time(nullptr));
    listItemRepository.update(id, listName, listItemToUpdate);
}

void
ListItemService::append(const std::string& id, ListName& listName, std::string itemValue)
{
    ListItemEntity listItemToUpdate(listName);
    listItemToUpdate = find(id, listName);

    // if last character is a space, remove it
    if ((*listItemToUpdate.getValue()).back() == ' ') {
        listItemToUpdate.setValue((*listItemToUpdate.getValue()).substr(0, (*listItemToUpdate.getValue()).size() - 1));
    }

    std::string newValue = *listItemToUpdate.getValue() + " " + std::move(itemValue);

    if (newValue.length() > 255) {
        throw std::invalid_argument("Item value must not exceed 255 characters.");
    }

    listItemToUpdate.setValue(newValue);
    listItemToUpdate.setUpdatedAt(time(nullptr));

    listItemRepository.update(id, listName, listItemToUpdate);
}

void
ListItemService::prepend(const std::string& id, ListName& listName, std::string itemValue)
{
    ListItemEntity listItemToUpdate(listName);
    listItemToUpdate = find(id, listName);

    // if last character is a space, remove it
    if ((*listItemToUpdate.getValue()).back() == ' ') {
        listItemToUpdate.setValue((*listItemToUpdate.getValue()).substr(0, (*listItemToUpdate.getValue()).size() - 1));
    }

    std::string newValue = std::move(itemValue) + " " + *listItemToUpdate.getValue();

    if (newValue.length() > 255) {
        throw std::invalid_argument("Item value must not exceed 255 characters.");
    }

    listItemToUpdate.setValue(newValue);
    listItemToUpdate.setUpdatedAt(time(nullptr));

    listItemRepository.update(id, listName, listItemToUpdate);
}

void
ListItemService::increasePriority(const std::string& id, ListName& listName)
{
    ListItemEntity listItemToUpdate(listName);
    listItemToUpdate = find(id, listName);

    if (priorityService.isMax(*(*listItemToUpdate.priority()).getName())) {
        throw std::invalid_argument("Priority of: " + id + " already at highest priority.");
    }

    int newPriority = *(*listItemToUpdate.priority()).getId() + 1;
    listItemToUpdate.setPriority(priorityService.find(newPriority));
    listItemToUpdate.setUpdatedAt(time(nullptr));

    listItemRepository.update(id, listName, listItemToUpdate);
}

void
ListItemService::decreasePriority(const std::string& id, ListName& listName)
{
    ListItemEntity listItemToUpdate(listName);
    listItemToUpdate = find(id, listName);

    if (priorityService.isMin(*(*listItemToUpdate.priority()).getName())) {
        throw std::invalid_argument("Priority of: " + id + " already at lowest priority.");
    }

    int newPriority = *(*listItemToUpdate.priority()).getId() - 1;
    listItemToUpdate.setPriority(priorityService.find(newPriority));
    listItemToUpdate.setUpdatedAt(time(nullptr));

    listItemRepository.update(id, listName, listItemToUpdate);
}

void
ListItemService::archiveAll(ListName& listName)
{
    std::vector<ListItemEntity> listItems = get(listName);
    for (ListItemEntity& listItem : listItems) {
        archiveItem(listItem, listName);
    }
}

void
ListItemService::archiveFinishedItems(ListName& listName)
{
    std::vector<ListItemEntity> listItems = get(listName);
    for (ListItemEntity& listItem : listItems) {
        if (*(*listItem.status()).isClosed()) {
            archiveItem(listItem, listName);
        }
    }
}

void
ListItemService::archiveItem(ListItemEntity& listItem, ListName& listName)
{
    ListName listNameArchive = ListName::createVariant(listName, "archive");
    listItemRepository.create(listItem, listNameArchive);
    listItemRepository.remove(*listItem.getId(), listName);
}

void
ListItemService::softDelete(const std::string& id, ListName& listName)
{
    ListName listNameDelete = ListName::createVariant(listName, "delete");
    ListItemEntity listItem = listItemRepository.find(id, listName);
    listItemRepository.create(listItem, listNameDelete);
    listItemRepository.remove(id, listName);
}

void
ListItemService::setPriority(const std::string& id, ListName& listName, const std::string* priorityName)
{
    ListItemEntity listItemToUpdate(listName);
    listItemToUpdate = find(id, listName);

    if (!priorityService.isNameValid(*priorityName)) {
        throw std::invalid_argument("Priority: " + *priorityName + " is not valid.");
    }

    if (*(*listItemToUpdate.priority()).getName() == *priorityName) {
        throw std::invalid_argument("Priority of: " + id + " already set to " + *priorityName);
    }

    PriorityEntity priorityEntity = priorityService.getPriorityFromName(*priorityName);
    listItemToUpdate.setPriority(priorityEntity);
    listItemToUpdate.setUpdatedAt(time(nullptr));

    listItemRepository.update(id, listName, listItemToUpdate);
}

void
ListItemService::setStatus(const std::string& id, ListName& listName, const std::string* statusName)
{
    ListItemEntity listItemToUpdate(listName);
    listItemToUpdate = find(id, listName);

    if (!statusService.isNameValid(*statusName)) {
        throw std::invalid_argument("Status: " + *statusName + " is not valid.");
    }

    if (*(*listItemToUpdate.status()).getName() == *statusName) {
        throw std::invalid_argument("Status of: " + id + " already set to " + *statusName);
    }

    StatusEntity statusEntity = statusService.getStatusFromName(*statusName);
    listItemToUpdate.setStatus(statusEntity);
    listItemToUpdate.setUpdatedAt(time(nullptr));

    listItemRepository.update(id, listName, listItemToUpdate);
}

std::vector<ListItemEntity>
ListItemService::search(ListName& listName, const std::vector<std::string>& searchValues)
{
    std::vector<ListItemEntity> listItems = get(listName);
    std::vector<ListItemEntity> foundItems;

    for (ListItemEntity& listItem : listItems) {
        for (const std::string& searchValue : searchValues) {
            if (searchValue.length() > 50) {
                throw std::invalid_argument("Item value must not exceed 50 characters.");
            }

            if (StringHelpers::containsString(
                    StringHelpers::filterAlnumAndSpace(StringHelpers::toLower(*listItem.getValue())),
                    StringHelpers::filterAlnumAndSpace(StringHelpers::toLower(searchValue)))) {
                foundItems.push_back(listItem);
                break;
            }
        }
    }

    if (foundItems.empty()) {
        throw std::invalid_argument("No items found with search values.");
    }

    return foundItems;
}

void
ListItemService::move(const std::string& id, ListName& oldListName, ListName& newListName)
{
    if (oldListName.getName() == newListName.getName()) {
        throw std::invalid_argument("Cannot move item to the same list.");
    }

    // Check if id exists in new list
    try {
        listItemRepository.find(id, newListName);
        listItemRepository.remove(id, newListName);
    } catch (std::exception& e) {
        // item does not exist in new list -> proceeding
    }

    ListItemEntity listItemToUpdate = listItemRepository.find(id, oldListName);
    listItemRepository.create(listItemToUpdate, newListName);
    listItemRepository.remove(id, oldListName);
}

void
ListItemService::copy(const std::string& id, ListName& oldListName, ListName& newListName)
{
    if (oldListName.getName() == newListName.getName()) {
        throw std::invalid_argument("Cannot copy item to the same list.");
    }

    // Check if id exists in new list
    try {
        listItemRepository.find(id, newListName);
        listItemRepository.remove(id, newListName);
    } catch (std::exception& e) {
        // item does not exist in new list -> proceeding
    }

    // If item exists in new list, remove it and copy the new one (it is an
    // overwrite)
    ListItemEntity listItemToUpdate = listItemRepository.find(id, oldListName);
    listItemRepository.create(listItemToUpdate, newListName);
}

void
ListItemService::duplicate(const std::string& id, ListName& listName)
{
    ListItemEntity listItemToDuplicate = listItemRepository.find(id, listName);
    std::string newId = makeId(listName);
    listItemToDuplicate.setId(newId);
    listItemRepository.create(listItemToDuplicate, listName);
}
void
ListItemService::restore(const std::string& id, ListName& listName)
{
    ListName listNameArchive = ListName::createVariant(listName, "archive");
    ListName listNameDelete = ListName::createVariant(listName, "delete");
    ListItemEntity listItem(listName);
    try {
        listItem = listItemRepository.find(id, listNameArchive);
        if (*listItem.getId() == id) {
            listItemRepository.create(listItem, listName);
            listItemRepository.remove(id, listNameArchive);
            return;
        }
    } catch (std::exception& e) {
        //
    }

    try {
        listItem = listItemRepository.find(id, listNameDelete);
        if (*listItem.getId() == id) {
            listItemRepository.create(listItem, listName);
            listItemRepository.remove(id, listNameDelete);
            return;
        }
    } catch (std::exception& e) {
        //
    }

    throw std::invalid_argument("Item to restore couldn't be found.");
}

void
ListItemService::archive(const std::string& id, ListName& listName)
{
    ListName listNameArchive = ListName::createVariant(listName, "archive");
    ListName listNameDelete = ListName::createVariant(listName, "delete");

    ListItemEntity listItem = listItemRepository.find(id, listName);
    listItemRepository.create(listItem, listNameArchive);
    listItemRepository.remove(id, listName);
}

void
ListItemService::editDeadline(std::string& id, ListName& listName, time_t dueAt)
{
    ListItemEntity listItemToUpdate(listName);

    try {
        listItemToUpdate = find(id, listName);
    } catch (std::exception& e) {
        throw std::invalid_argument("Item not found.");
    }

    if (*(*listItemToUpdate.status()).isClosed()) {
        throw std::invalid_argument("Cannot set deadline for a closed item.");
    }

    try {
        listItemToUpdate.setDueAt(dueAt);
        listItemToUpdate.setUpdatedAt(time(nullptr));
        listItemRepository.update(id, listName, listItemToUpdate);
    } catch (std::exception& e) {
        throw std::invalid_argument("Deadline could not be set.");
    }
}

PriorityService&
ListItemService::priority()
{
    return priorityService;
}

StatusService&
ListItemService::status()
{
    return statusService;
}

long
ListItemService::count(ListName& listName)
{
    // TODO: Optimize this request
    std::vector<ListItemEntity> listItems = listItemRepository.get(listName);
    return static_cast<long>(listItems.size());
}

long
ListItemService::countWithStatus(ListName& listName, const std::vector<int>& status)
{
    long count = 0;
    for (int statusId : status) {
        if (!statusService.isIdValid(statusId)) {
            throw std::invalid_argument("Status does not exists.");
        }

        // TODO: Optimize this request
        std::vector<ListItemEntity> listItems = listItemRepository.get(listName);
        for (ListItemEntity& listItem : listItems) {
            if (*(*listItem.status()).getId() == statusId) {
                count++;
            }
        }
    }

    return count;
}

long
ListItemService::countWithPriority(ListName& listName, const std::vector<int>& priorities)
{
    long count = 0;
    for (int priorityId : priorities) {
        if (!priorityService.isIdValid(priorityId)) {
            throw std::invalid_argument("Priority does not exists.");
        }

        // TODO: Optimize this request
        std::vector<ListItemEntity> listItems = listItemRepository.get(listName);
        for (ListItemEntity& listItem : listItems) {
            if (*(*listItem.priority()).getId() == priorityId) {
                count++;
            }
        }
    }

    return count;
}

long
ListItemService::countCreatedBetween(ListName& listName, time_t from, time_t to)
{
    ListName listNameArchive = ListName::createVariant(listName, "archive");

    long count = 0;
    std::vector<ListItemEntity> listItems = listItemRepository.get(listName);
    std::vector<ListItemEntity> listItemsArchive = listItemRepository.get(listNameArchive);
    listItems.insert(listItems.end(), listItemsArchive.begin(), listItemsArchive.end());
    for (ListItemEntity& listItem : listItems) {
        if (*listItem.getCreatedAt() >= from && *listItem.getCreatedAt() <= to) {
            count++;
        }
    }
    return count;
}

long
ListItemService::countClosedBetween(ListName& listName, time_t from, time_t to)
{
    ListName listNameArchive = ListName::createVariant(listName, "archive");

    long count = 0;
    std::vector<ListItemEntity> listItems = listItemRepository.get(listName);
    std::vector<ListItemEntity> listItemsArchive = listItemRepository.get(listNameArchive);
    listItems.insert(listItems.end(), listItemsArchive.begin(), listItemsArchive.end());
    for (ListItemEntity& listItem : listItems) {
        if (*listItem.getClosedAt() > 0 && *listItem.getClosedAt() >= from && *listItem.getClosedAt() <= to) {
            count++;
        }
    }
    return count;
}

std::vector<ListItemEntity>
ListItemService::sort(std::vector<ListItemEntity> listItems)
{
    // We sort by priority
    // Otherwise we sort by status (cancelled at the end)
    // If status is the same, we sort by creaton date
    std::sort(listItems.begin(), listItems.end(), [](const ListItemEntity& a, const ListItemEntity& b) {
        // if a is cancelled and b is not, b is first
        if (*(*a.status()).isCancelled() && !*(*b.status()).isCancelled()) {
            return false;
        }
        // if b is cancelled and a is not, a is first
        else if (!*(*a.status()).isCancelled() && *(*b.status()).isCancelled()) {
            return true;
        }
        // if both are cancelled, sort by creation date
        else if (*(*a.status()).isCancelled() && *(*b.status()).isCancelled()) {
            return *a.getCreatedAt() < *b.getCreatedAt();
        }
        // if priority is different, sort by priority
        else if (*(*a.priority()).getPosition() != *(*b.priority()).getPosition()) {
            return *(*a.priority()).getPosition() > *(*b.priority()).getPosition();
        }
        // if status is different and both are not closed, sort by status
        else if (*(*a.status()).getCommandName() != *(*b.status()).getCommandName()) {
            if (*(*a.status()).getCommandName() == "started") {
                return true;
            } else if (*(*b.status()).getCommandName() == "started") {
                return false;
            } else {
                if (*(*a.status()).getPosition() != *(*b.status()).getPosition()) {
                    return *(*a.status()).getPosition() < *(*b.status()).getPosition();
                } else {
                    return *a.getCreatedAt() > *b.getCreatedAt();
                }
            }
        } else if (*a.getDueAt() != *b.getDueAt()) {
            return *a.getDueAt() > *b.getDueAt();
        }

        return *a.getCreatedAt() > *b.getCreatedAt();
    });

    return listItems;
}

void
ListItemService::filterPriorityAbove(std::vector<ListItemEntity>& listItems, const int priority)
{
    listItems.erase(
        std::remove_if(listItems.begin(),
                       listItems.end(),
                       [priority](const ListItemEntity& item) { return *(*item.priority()).getPosition() < priority; }),
        listItems.end());
}

void
ListItemService::filterStatus(std::vector<ListItemEntity>& listItems, const std::vector<int>& statuses)
{
    listItems.erase(std::remove_if(listItems.begin(),
                                   listItems.end(),
                                   [statuses, this](const ListItemEntity& item) {
                                       return std::find(statuses.begin(),
                                                        statuses.end(),
                                                        *(*item.status()).getPosition()) == statuses.end();
                                   }),
                    listItems.end());
}

void
ListItemService::filterDeadlineBefore(std::vector<ListItemEntity>& listItems, const time_t timestamp)
{
    listItems.erase(std::remove_if(listItems.begin(),
                                   listItems.end(),
                                   [timestamp](const ListItemEntity& item) {
                                       if (*item.getDueAt() == 0) {
                                           return true;
                                       }

                                       return *item.getDueAt() >= timestamp + 86400;
                                   }),
                    listItems.end());
}
