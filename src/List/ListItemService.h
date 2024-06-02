#ifndef LISTITEMSERVICE_H
#define LISTITEMSERVICE_H

#include "../FileDataStorageRepositories/ListItemRepository.h"
#include "../IOService/IOService.h"
#include "../Events/EventBus.h"
#include "../Helpers/DateHelpers.h"
#include "../Helpers/StringHelpers.h"

class ListItemService {
public:
    ListItemService(IOService& ioService,
                    ConfigService& configService,
                    ListItemRepository& listItemRepository,
                    PriorityService& priorityService,
                    StatusService& statusService,
                    EventBus& bus
                    );
    PriorityService& priority();
    StatusService& status();
    ListItemService& load(std::string name = "", std::string variant = "default");
    ListItemService& loadVariant(std::string variant = "default");
    std::vector <ListItemEntity> get();
    void filterPriorityAbove(std::vector <ListItemEntity>& listItems, const int priority);
    void filterStatus(std::vector <ListItemEntity>& listItems, const std::vector <int>& statuses);
    void filterDeadlineBefore(std::vector <ListItemEntity>& listItems, const time_t timestamp);
    ListItemEntity find(const std::string& id);
    bool remove(const std::string& id);
    void softDelete(const std::string& id);
    std::string add(const std::string& itemValue, const std::string* priority = nullptr, const std::string* status = nullptr, time_t dueAt = 0);
    void edit(const std::string& id, const std::string& itemValue, const std::string* priority = nullptr, const std::string* status = nullptr);
    void editStatus(const std::string& id, const int* status);
    void reset (const std::string& id);
    std::vector <ListItemEntity> search(const std::vector <std::string>& searchValues);
    void append(const std::string& id, std::string itemValue);
    void prepend(const std::string& id, std::string itemValue);
    void increasePriority(const std::string& id);
    void decreasePriority(const std::string& id);
    void setPriority(const std::string& id, const std::string* priorityName);
    void setStatus(const std::string& id, const std::string* statusName);
    void move(const std::string& id, const std::string& oldListName, const std::string& newListName);
    void copy(const std::string& id, const std::string& oldListName, const std::string& newListName);
    void archive(const std::string& id);
    void archiveAll();
    void archiveFinishedItems();
    void subscribeToEvents(EventBus& eventBus);
    void restore(const std::string& id);
    void editDeadline(std::string& id, time_t dueAt = 0);
    long count();
    long countWithStatus(const std::vector <int>& status);
    long countWithPriority(const std::vector <int>& priorities);
    long countCreatedBetween(time_t from, time_t to);
    long countClosedBetween(time_t from, time_t to);
private:
    IOService& ioService;
    ConfigService& configService;
    ListItemRepository& listItemRepository;
    std::string listName;
    std::string listVariant;
    PriorityService priorityService;
    StatusService statusService;
    EventBus& bus;
    std::vector <ListItemEntity> sort(std::vector <ListItemEntity> listItems);
protected:
    void archive(ListItemEntity &listItem);
};



#endif //LISTITEMSERVICE_H
