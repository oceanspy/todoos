#include "ListService.h"


ListService::ListService(IOService& ioService, ConfigService& configService, ListRepository& listRepository, EventBus& bus)
    : ioService(ioService), configService(configService), listRepository(listRepository), bus(bus)
{
    subscribeToEvents(bus);
}

std::vector <ListEntity> ListService::get(bool keepHidden)
{
    std::vector <ListEntity> lists = listRepository.get();

    // lists erase hidden elements
    if (!keepHidden)
    {
        lists.erase(std::remove_if(lists.begin(), lists.end(), [](const ListEntity& listEntity) {
            return *listEntity.isHidden();
        }), lists.end());
    }

    std::vector <ListEntity> sortedItems = sort(lists);
    return sortedItems;
}

ListEntity ListService::find(const std::string& listName) {
    return listRepository.find(listName);
}

void ListService::add(const std::string& listName, const std::string& type, const std::string& sorting)
{
    validateListName(listName);

    ListEntity foundList;
    try {
        foundList = listRepository.find(listName);
    } catch (std::exception &e) {
        // Continue
    }

    if (foundList.getName() != nullptr)
    {
        throw std::invalid_argument("List with name: " + listName + " already exists.");
    }

    ListEntity listEntity = ListEntity();
    listEntity.setName(listName);
    listEntity.setType(type);
    listEntity.setSorting(sorting);
    listRepository.create(listEntity);
}

void ListService::edit(const std::string& oldListName, std::string newListName)
{
    validateListName(newListName);

    ListEntity listEntity = find(oldListName);
    listEntity.setName(std::move(newListName));
    listRepository.update(oldListName, listEntity);
}

void ListService::remove(std::string& listName)
{
    listRepository.remove(listName);
}

bool ListService::use(const std::string& listName)
{
    std::vector <ListEntity> listItems = get(true);

    auto listFound = std::any_of(listItems.begin(), listItems.end(), [&](const ListEntity& listItem) {
        return *listItem.getName() == listName;
    });

    if (listFound) {
        configService.edit("currentList", listName);
        // Publishing an event to the event bus
        bus.publish(Event("currentListUpdated", listName));
        return true;
    }

    return false;
}

void ListService::subscribeToEvents(EventBus& eventBus)
{
    eventBus.subscribe("listUpdated", [this](Event event) {
        // React to the event here
    });
}

std::string ListService::getType(const std::string& listName) {
    ListEntity listEntity = find(listName);
    return *listEntity.getType();
}

bool ListService::isListExist(const std::string &listName)
{
    try {
        listRepository.find(listName);
    } catch (std::exception &e) {
        return false;
    }
    return true;
}

void ListService::validateListName(const std::string &newListName)
{
    if (!std::regex_match(newListName, std::regex("^[a-zA-Z0-9_-]*$")))
    {
        throw std::invalid_argument("List name must be alphanumeric and can contain _.-");
    }
    if (newListName.length() > 50)
    {
        throw std::invalid_argument("List name must not exceed 50 characters.");
    }
}

std::vector <ListEntity> ListService::sort(std::vector <ListEntity> listItems)
{
    std::sort(listItems.begin(), listItems.end(), [](ListEntity &a, ListEntity &b) {
        return *a.getName() < *b.getName();
    });

    return listItems;
}
