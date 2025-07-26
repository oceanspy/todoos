#ifndef LISTITEMSERVICE_H
#define LISTITEMSERVICE_H

#include "../Events/EventBus.h"
#include "../FileDataStorageRepositories/ListItemRepository.h"
#include "../Helpers/DateHelpers.h"
#include "../Helpers/StringHelpers.h"
#include "../IOService/IOService.h"
#include "ListName.h"

class ListItemService
{
  public:
    ListItemService(IOService& ioService,
                    ConfigService& configService,
                    ListItemRepository& listItemRepository,
                    PriorityService& priorityService,
                    StatusService& statusService);
    PriorityService& priority();
    StatusService& status();
    std::vector<ListItemEntity> get(ListName& listName);
    ListItemEntity find(const std::string& id, ListName& listName);
    bool remove(const std::string& id, ListName& listName);
    void softDelete(const std::string& id, ListName& listName);
    std::string add(ListName& listName,
                    const std::string& itemValue,
                    const std::string* priority = nullptr,
                    const std::string* status = nullptr,
                    time_t dueAt = 0);
    std::string makeId(ListName& listName);
    bool isIdAvailable(const std::string& id, ListName& listName);
    void edit(const std::string& id,
              ListName& listName,
              const std::string& itemValue,
              const std::string* priority = nullptr,
              const std::string* status = nullptr);
    void editStatus(const std::string& id, ListName& listName, const int* status);
    void reset(const std::string& id, ListName& listName);
    std::vector<ListItemEntity> search(ListName& listName, const std::vector<std::string>& searchValues);
    void append(const std::string& id, ListName& listName, std::string itemValue);
    void prepend(const std::string& id, ListName& listName, std::string itemValue);
    void increasePriority(const std::string& id, ListName& listName);
    void decreasePriority(const std::string& id, ListName& listName);
    void setPriority(const std::string& id, ListName& listName, const std::string* priorityName);
    void setStatus(const std::string& id, ListName& listName, const std::string* statusName);
    void move(const std::string& id, ListName& oldListName, ListName& newListName);
    void copy(const std::string& id, ListName& oldListName, ListName& newListName);
    void duplicate(const std::string& id, ListName& listName);
    void archive(const std::string& id, ListName& listName);
    void archiveAll(ListName& listName);
    void archiveFinishedItems(ListName& listName);
    void restore(const std::string& id, ListName& listName);
    void editDeadline(std::string& id, ListName& listName, time_t dueAt = 0);
    long count(ListName& listName);
    long countWithStatus(ListName& listName, const std::vector<int>& status);
    long countWithPriority(ListName& listName, const std::vector<int>& priorities);
    long countCreatedBetween(ListName& listName, time_t from, time_t to);
    long countClosedBetween(ListName& listName, time_t from, time_t to);
    void filterPriorityAbove(std::vector<ListItemEntity>& listItems, const int priority);
    void filterStatus(std::vector<ListItemEntity>& listItems, const std::vector<int>& statuses);
    void filterDeadlineBefore(std::vector<ListItemEntity>& listItems, const time_t timestamp);
    const int idLength = 4;
    const std::string idLetters = "letters";
    const std::string idLettersLowercase = "letters-lowercase";
    const std::string idRandom = "random";

  private:
    IOService& ioService;
    ConfigService& configService;
    ListItemRepository& listItemRepository;
    PriorityService priorityService;
    StatusService statusService;
    std::vector<ListItemEntity> sort(std::vector<ListItemEntity> listItems);

  protected:
    void archiveItem(ListItemEntity& listItem, ListName& listName);
};

#endif // LISTITEMSERVICE_H
