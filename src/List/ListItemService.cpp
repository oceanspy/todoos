#include "ListItemService.h"

#include <string>
#include <utility>

ListItemService::ListItemService(IOService& ioService, ConfigService& configService, ListItemRepository& listItemRepository, PriorityService& priorityService, StatusService& statusService, EventBus& bus)
    : ioService(ioService), configService(configService), listItemRepository(listItemRepository), priorityService(priorityService), statusService(statusService), bus(bus)
{
    subscribeToEvents(bus);
    load();
}

ListItemService &ListItemService::load(std::string name, std::string variant)
{
    listName = std::move(name);
    listVariant = std::move(variant);
    listItemRepository.load(listName, listVariant);
    return *this;
}

ListItemService &ListItemService::loadVariant(std::string variant)
{
    listVariant = std::move(variant);
    listItemRepository.loadVariant(listVariant);
    return *this;
}

std::vector <ListItemEntity> ListItemService::get()
{
    return sort(listItemRepository.load(listName, listVariant).get());
}

std::vector <ListItemEntity> ListItemService::sort(std::vector <ListItemEntity> listItems)
{
    // We sort by priority
    // Otherwise we sort by status (cancelled at the end)
    // If status is the same, we sort by creaton date
    std::sort(listItems.begin(), listItems.end(), [](const ListItemEntity &a, const ListItemEntity &b) {
        // if a is cancelled and b is not, b is first
        if (*(*a.status()).isCancelled() && !*(*b.status()).isCancelled())
        {
            return false;
        }
        // if b is cancelled and a is not, a is first
        else if (!*(*a.status()).isCancelled() && *(*b.status()).isCancelled())
        {
            return true;
        }
        // if both are cancelled, sort by creation date
        else if (*(*a.status()).isCancelled() && *(*b.status()).isCancelled())
        {
            return *a.getCreatedAt() < *b.getCreatedAt();
        }
        // if priority is different, sort by priority
        else if (*(*a.priority()).getPosition() != *(*b.priority()).getPosition())
        {
            return *(*a.priority()).getPosition() > *(*b.priority()).getPosition();
        }
        // if status is different and both are not closed, sort by status
        else if (*(*a.status()).getCommandName() != *(*b.status()).getCommandName())
        {
            if (*(*a.status()).getCommandName() == "started") {
                return true;
            } else if (*(*b.status()).getCommandName() == "started") {
                return false;
            } else {
                if (*(*a.status()).getPosition() != *(*b.status()).getPosition()) {
                    return *(*a.status()).getPosition() < *(*b.status()).getPosition();
                }
                else
                {
                    return *a.getCreatedAt() > *b.getCreatedAt();
                }
            }
        }
        else if (*a.getDueAt() != *b.getDueAt())
        {
            return *a.getDueAt() > *b.getDueAt();
        }

        return *a.getCreatedAt() > *b.getCreatedAt();
    });

    return listItems;
}

void ListItemService::filterPriorityAbove(std::vector <ListItemEntity>& listItems, const int priority)
{
    listItems.erase(std::remove_if(listItems.begin(), listItems.end(), [priority](const ListItemEntity& item) {
        return *(*item.priority()).getPosition() < priority;
    }), listItems.end());
}

void ListItemService::filterStatus(std::vector <ListItemEntity>& listItems, const std::vector <int>& statuses)
{
    listItems.erase(std::remove_if(listItems.begin(), listItems.end(), [statuses, this](const ListItemEntity& item) {
        return std::find(statuses.begin(), statuses.end(), *(*item.status()).getPosition()) == statuses.end();
    }), listItems.end());
}

void ListItemService::filterDeadlineBefore(std::vector <ListItemEntity>& listItems, const time_t timestamp)
{
    listItems.erase(std::remove_if(listItems.begin(), listItems.end(), [timestamp](const ListItemEntity& item) {
        if (*item.getDueAt() == 0)
        {
            return true;
        }

        return *item.getDueAt() >= timestamp + 86400;
    }), listItems.end());
}

ListItemEntity ListItemService::find(const std::string& id)
{
    return listItemRepository.load(listName, listVariant).find(id);
}

bool ListItemService::remove(const std::string& id)
{
    return listItemRepository.load(listName, listVariant).remove(id);
}

std::string ListItemService::add(const std::string& itemValue, const std::string* priority, const std::string* status, time_t dueAt)
{
    if (itemValue.length() > 255)
    {
        throw std::invalid_argument("Item value must not exceed 255 characters.");
    }

    std::string id = makeId();

    ListItemEntity listItemEntity;
    listItemEntity.setId(id);

    if (priority != nullptr)
    {
        PriorityEntity priorityEntity = priorityService.getPriorityFromName(*priority);
        listItemEntity.setPriority(priorityEntity);
    } else
    {
        PriorityEntity priorityEntity = priorityService.getPriorityFromName("medium");
        listItemEntity.setPriority(priorityEntity);
    }
    if (status != nullptr)
    {
        StatusEntity statusEntity = statusService.getStatusFromName(*status);
        listItemEntity.setStatus(statusEntity);
    } else
    {
        StatusEntity statusEntity = statusService.getStatusFromName("to-do");
        listItemEntity.setStatus(statusEntity);
    }

    listItemEntity.setValue(itemValue);
    listItemEntity.setCreatedAt(time(nullptr));
    listItemEntity.setUpdatedAt(time(nullptr));
    listItemEntity.setDueAt(dueAt);
    listItemRepository.load(listName, listVariant).create(listItemEntity);

    return id;
}

std::string ListItemService::makeId()
{
    bool validId = false;
    std::string id;
    while (!validId)
    {
        if (configService.getValue("useOnlyLettersForIds") == "true") {
            id = StringHelpers::randomString();
        } else {
            id = StringHelpers::randomAlNumString();
        }
        
        if(isIdAvailable(id)) {
            validId = true;
        }
    }

    return id;
}

bool ListItemService::isIdAvailable(const std::string& id)
{
    // TO DO: Optimize this
    try
    {
        find(id);
        return false;
    }
    catch (std::exception &e)
    {
        // id is not in default list
    }

    try
    {
        loadVariant("archive").find(id);
        return false;
    }
    catch (std::exception &e)
    {
        // id is not in archive list
    }

    try
    {
        loadVariant("delete").find(id);
        return false;
    }
    catch (std::exception &e)
    {
        // id is not in delete list
    }

    loadVariant();
    return true;
}

void ListItemService::edit(const std::string& id, const std::string& itemValue, const std::string* priority, const std::string* status)
{
    if (itemValue.length() > 255)
    {
        throw std::invalid_argument("Item value must not exceed 255 characters.");
    }

    if (itemValue.empty() && priority == nullptr && status == nullptr)
    {
        throw std::invalid_argument("No values to update.");
    }

    ListItemEntity listItemToUpdate;
    listItemToUpdate = find(id);

    if (priority != nullptr)
    {
        PriorityEntity priorityEntity = priorityService.getPriorityFromName(*priority);
        listItemToUpdate.setPriority(priorityEntity);
    }
    if (status != nullptr)
    {
        StatusEntity statusEntity = statusService.getStatusFromName(*status);
        listItemToUpdate.setStatus(statusEntity);
    }

    if (!itemValue.empty())
    {
        listItemToUpdate.setValue(itemValue);
    }
    listItemToUpdate.setUpdatedAt(time(nullptr));

    listItemRepository.load(listName, listVariant).update(id, listItemToUpdate);
}

void ListItemService::editStatus(const std::string& id, const int *status)
{
    ListItemEntity listItemToUpdate;
    listItemToUpdate = find(id);
    // check if status is not already set
    if (*(*listItemToUpdate.status()).getId() == *status)
    {
        throw std::invalid_argument("Status of: " + id + " already set to: " + std::to_string(*status));
    }

    StatusEntity statusEntity = statusService.find(*status);
    listItemToUpdate.setStatus(statusEntity);
    listItemToUpdate.setUpdatedAt(time(nullptr));

    if (statusService.isClosed(statusService.getNameFromId(*status)))
    {
        listItemToUpdate.setClosedAt(time(nullptr));
    } else {
        listItemToUpdate.setClosedAt(0);
    }

    listItemRepository.load(listName, listVariant).update(id, listItemToUpdate);

    if (configService.isTrue("archiveWhenCompleted"))
    {
        if (statusService.isClosed(statusService.getNameFromId(*status)))
        {
            archive(id);
        }
    }
}

void ListItemService::reset(const std::string& id)
{
    ListItemEntity listItemToUpdate;
    listItemToUpdate = find(id);
    StatusEntity statusEntity = statusService.getStatusFromName("to-do");
    listItemToUpdate.setStatus(statusEntity);
    listItemToUpdate.setCreatedAt(time(nullptr));
    listItemToUpdate.setUpdatedAt(time(nullptr));
    listItemRepository.load(listName, listVariant).update(id, listItemToUpdate);
}

void ListItemService::append(const std::string& id, std::string itemValue)
{
    ListItemEntity listItemToUpdate;
    listItemToUpdate = find(id);

    // if last character is a space, remove it
    if ((*listItemToUpdate.getValue()).back() == ' ')
    {
        listItemToUpdate.setValue((*listItemToUpdate.getValue()).substr(0, (*listItemToUpdate.getValue()).size() - 1));
    }

    std::string newValue = *listItemToUpdate.getValue() + " " + std::move(itemValue);

    if (newValue.length() > 255)
    {
        throw std::invalid_argument("Item value must not exceed 255 characters.");
    }

    listItemToUpdate.setValue(newValue);
    listItemToUpdate.setUpdatedAt(time(nullptr));

    listItemRepository.load(listName, listVariant).update(id, listItemToUpdate);
}

void ListItemService::prepend(const std::string& id, std::string itemValue)
{
    ListItemEntity listItemToUpdate;
    listItemToUpdate = find(id);

    // if last character is a space, remove it
    if ((*listItemToUpdate.getValue()).back() == ' ')
    {
        listItemToUpdate.setValue((*listItemToUpdate.getValue()).substr(0, (*listItemToUpdate.getValue()).size() - 1));
    }

    std::string newValue = std::move(itemValue) + " " + *listItemToUpdate.getValue();

    if (newValue.length() > 255)
    {
        throw std::invalid_argument("Item value must not exceed 255 characters.");
    }

    listItemToUpdate.setValue(newValue);
    listItemToUpdate.setUpdatedAt(time(nullptr));

    listItemRepository.load(listName, listVariant).update(id, listItemToUpdate);
}

void ListItemService::increasePriority(const std::string& id)
{
    ListItemEntity listItemToUpdate;
    listItemToUpdate = find(id);

    if (priorityService.isMax(*(*listItemToUpdate.priority()).getName()))
    {
        throw std::invalid_argument("Priority of: " + id + " already at highest priority.");
    }

    int newPriority = *(*listItemToUpdate.priority()).getId() + 1;
    listItemToUpdate.setPriority(priorityService.find(newPriority));
    listItemToUpdate.setUpdatedAt(time(nullptr));

    listItemRepository.load(listName, listVariant).update(id, listItemToUpdate);
}

void ListItemService::decreasePriority(const std::string& id)
{
    ListItemEntity listItemToUpdate;
    listItemToUpdate = find(id);

    if (priorityService.isMin(*(*listItemToUpdate.priority()).getName()))
    {
        throw std::invalid_argument("Priority of: " + id + " already at lowest priority.");
    }

    int newPriority = *(*listItemToUpdate.priority()).getId() - 1;
    listItemToUpdate.setPriority(priorityService.find(newPriority));
    listItemToUpdate.setUpdatedAt(time(nullptr));

    listItemRepository.load(listName, listVariant).update(id, listItemToUpdate);
}

void ListItemService::archiveAll()
{
    std::vector <ListItemEntity> listItems = get();
    for (ListItemEntity& listItem : listItems)
    {
        archive(listItem);
    }
}

void ListItemService::archiveFinishedItems()
{
    std::vector <ListItemEntity> listItems = get();
    for (ListItemEntity& listItem : listItems)
    {
        if (*(*listItem.status()).isClosed())
        {
            archive(listItem);
        }
    }
}

void ListItemService::archive(ListItemEntity &listItem)
{
    listItemRepository.load(listName, "archive").create(listItem);
    listItemRepository.load(listName, "default").remove(*listItem.getId());
}

void ListItemService::softDelete(const std::string& id)
{
    ListItemEntity listItem = listItemRepository.load(listName).find(id);
    listItemRepository.load(listName, "delete").create(listItem);
    listItemRepository.load(listName, "default").remove(id);
}

void ListItemService::setPriority(const std::string& id, const std::string *priorityName)
{
    ListItemEntity listItemToUpdate;
    listItemToUpdate = find(id);

    if (!priorityService.isNameValid(*priorityName))
    {
        throw std::invalid_argument("Priority: " + *priorityName + " is not valid.");
    }

    if (*(*listItemToUpdate.priority()).getName() == *priorityName)
    {
        throw std::invalid_argument("Priority of: " + id + " already set to " + *priorityName);
    }

    PriorityEntity priorityEntity = priorityService.getPriorityFromName(*priorityName);
    listItemToUpdate.setPriority(priorityEntity);
    listItemToUpdate.setUpdatedAt(time(nullptr));

    listItemRepository.load(listName, listVariant).update(id, listItemToUpdate);
}

void ListItemService::setStatus(const std::string& id, const std::string *statusName)
{
    ListItemEntity listItemToUpdate;
    listItemToUpdate = find(id);

    if (!statusService.isNameValid(*statusName))
    {
        throw std::invalid_argument("Status: " + *statusName + " is not valid.");
    }

    if (*(*listItemToUpdate.status()).getName() == *statusName)
    {
        throw std::invalid_argument("Status of: " + id + " already set to " + *statusName);
    }

    StatusEntity statusEntity = statusService.getStatusFromName(*statusName);
    listItemToUpdate.setStatus(statusEntity);
    listItemToUpdate.setUpdatedAt(time(nullptr));

    listItemRepository.load(listName, listVariant).update(id, listItemToUpdate);
}

std::vector<ListItemEntity> ListItemService::search(const std::vector<std::string>& searchValues)
{
    std::vector <ListItemEntity> listItems = load(listName, listVariant).get();
    std::vector <ListItemEntity> foundItems;

    for (ListItemEntity& listItem : listItems)
    {
        for (const std::string& searchValue : searchValues)
        {
            if (searchValue.length() > 50)
            {
                throw std::invalid_argument("Item value must not exceed 50 characters.");
            }

            if (StringHelpers::containsString(
                    StringHelpers::filterAlnumAndSpace(StringHelpers::toLower(*listItem.getValue())),
                    StringHelpers::filterAlnumAndSpace(StringHelpers::toLower(searchValue)))
            ) {
                foundItems.push_back(listItem);
                break;
            }
        }
    }

    if (foundItems.empty())
    {
        throw std::invalid_argument("No items found with search values.");
    }

    return foundItems;
}

void ListItemService::subscribeToEvents(EventBus& eventBus)
{
    eventBus.subscribe("currentListUpdated", [this](Event event)
    {
        // React to the event here
        load(event.getPayload());
    });
}

void ListItemService::move(const std::string& id, const std::string& oldListName, const std::string& newListName)
{
    if (oldListName == newListName)
    {
        throw std::invalid_argument("Cannot move item to the same list.");
    }

    // Check if id exists in new list
    try {
        listItemRepository.load(newListName).find(id);
    } catch (std::exception &e) {
        // item does not exist in new list -> proceeding
        ListItemEntity listItemToUpdate = listItemRepository.load(oldListName).find(id);
        listItemRepository.load(newListName).create(listItemToUpdate);
        listItemRepository.load(oldListName).remove(id);
        return;
    }

    // If item exists in new list, remove it and move the new one (it is an overwrite)
    listItemRepository.load(newListName).remove(id);
    ListItemEntity listItemToUpdate = listItemRepository.load(oldListName).find(id);
    listItemRepository.load(newListName).create(listItemToUpdate);
    listItemRepository.load(oldListName).remove(id);
}

void ListItemService::copy(const std::string& id, const std::string& oldListName, const std::string& newListName)
{
    if (oldListName == newListName)
    {
        throw std::invalid_argument("Cannot copy item to the same list.");
    }

    // Check if id exists in new list
    try {
        listItemRepository.load(newListName).find(id);
    } catch (std::exception &e) {
        // item does not exist in new list -> proceeding
        ListItemEntity listItemToUpdate = listItemRepository.load(oldListName).find(id);
        listItemRepository.load(newListName).create(listItemToUpdate);
        return;
    }

    // If item exists in new list, remove it and copy the new one (it is an overwrite)
    listItemRepository.load(newListName).remove(id);
    ListItemEntity listItemToUpdate = listItemRepository.load(oldListName).find(id);
    listItemRepository.load(newListName).create(listItemToUpdate);
}

void ListItemService::duplicate(const std::string& id, const std::string& listName)
{
    ListItemEntity listItemToDuplicate = listItemRepository.load(listName).find(id);
    std::string newId = makeId();
    listItemToDuplicate.setId(newId);
    listItemRepository.load(listName).create(listItemToDuplicate);
}
void ListItemService::restore(const std::string& id)
{
    ListItemEntity listItem;
    try {
        listItem = listItemRepository.load(listName, "archive").find(id);
        if (*listItem.getId() == id)
        {
            listItemRepository.load(listName, "default").create(listItem);
            listItemRepository.load(listName, "archive").remove(id);
            return;
        }
    } catch (std::exception &e) {
        //
    }

    try {
        listItem = listItemRepository.load(listName, "delete").find(id);
        if (*listItem.getId() == id)
        {
            listItemRepository.load(listName, "default").create(listItem);
            listItemRepository.load(listName, "delete").remove(id);
            return;
        }
    } catch (std::exception &e) {
        //
    }

    throw std::invalid_argument("Item to restore couldn't be found.");
}

void ListItemService::archive(const std::string &id)
{
    ListItemEntity listItem = listItemRepository.load(listName, "default").find(id);
    listItemRepository.load(listName, "archive").create(listItem);
    listItemRepository.load(listName, "default").remove(id);
}

void ListItemService::editDeadline(std::string& id, time_t dueAt)
{
    ListItemEntity listItemToUpdate;

    try {
        listItemToUpdate = find(id);
    } catch (std::exception &e) {
        throw std::invalid_argument("Item not found.");
    }

    if (*(*listItemToUpdate.status()).isClosed())
    {
        throw std::invalid_argument("Cannot set deadline for a closed item.");
    }

    try {
        listItemToUpdate.setDueAt(dueAt);
        listItemToUpdate.setUpdatedAt(time(nullptr));
        listItemRepository.load(listName, listVariant).update(id, listItemToUpdate);
    } catch (std::exception &e) {
        throw std::invalid_argument("Deadline could not be set.");
    }
}

PriorityService &ListItemService::priority()
{
    return priorityService;
}

StatusService &ListItemService::status()
{
    return statusService;
}

long ListItemService::count()
{
    // TODO: Optimize this request
    std::vector <ListItemEntity> listItems = listItemRepository.load(listName, listVariant).get();
    return static_cast<long>(listItems.size());
}

long ListItemService::countWithStatus(const std::vector <int>& status)
{
    long count = 0;
    for (int statusId : status)
    {
        if (!statusService.isIdValid(statusId))
        {
            throw std::invalid_argument("Status does not exists.");
        }

        // TODO: Optimize this request
        std::vector <ListItemEntity> listItems = listItemRepository.load(listName, listVariant).get();
        for (ListItemEntity& listItem : listItems)
        {
            if (*(*listItem.status()).getId() == statusId)
            {
                count++;
            }
        }
    }

    return count;
}

long ListItemService::countWithPriority(const std::vector <int>& priorities)
{
    long count = 0;
    for (int priorityId : priorities) {
        if (!priorityService.isIdValid(priorityId)) {
            throw std::invalid_argument("Priority does not exists.");
        }

        // TODO: Optimize this request
        std::vector<ListItemEntity> listItems = listItemRepository.load(listName, listVariant).get();
        for (ListItemEntity &listItem: listItems) {
            if (*(*listItem.priority()).getId() == priorityId) {
                count++;
            }
        }
    }

    return count;
}

long ListItemService::countCreatedBetween(time_t from, time_t to)
{
    long count = 0;
    std::vector<ListItemEntity> listItems = listItemRepository.load(listName, "default").get();
    std::vector<ListItemEntity> listItemsArchive = listItemRepository.load(listName, "archive").get();
    listItems.insert(listItems.end(), listItemsArchive.begin(), listItemsArchive.end());
    for (ListItemEntity &listItem: listItems) {
        if (*listItem.getCreatedAt() >= from && *listItem.getCreatedAt() <= to) {
            count++;
        }
    }
    return count;
}

long ListItemService::countClosedBetween(time_t from, time_t to)
{
    long count = 0;
    std::vector<ListItemEntity> listItems = listItemRepository.load(listName, "default").get();
    std::vector<ListItemEntity> listItemsArchive = listItemRepository.load(listName, "archive").get();
    listItems.insert(listItems.end(), listItemsArchive.begin(), listItemsArchive.end());
    for (ListItemEntity &listItem: listItems) {
        if (*listItem.getClosedAt() > 0 && *listItem.getClosedAt() >= from && *listItem.getClosedAt() <= to) {
            count++;
        }
    }
    return count;
}
