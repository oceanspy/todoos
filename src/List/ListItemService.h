#ifndef LISTITEMSERVICE_H
#define LISTITEMSERVICE_H

#include "../Events/EventBus.h"
#include "../FileDataStorageRepositories/DescriptionRepository.h"
#include "../FileDataStorageRepositories/ListItemRepository.h"
#include "../Helpers/DateHelpers.h"
#include "../Helpers/StringHelpers.h"
#include "../IOService/IOService.h"
#include "ListCountSummary.h"
#include "ListItemId.h"
#include "ListName.h"

class ListItemService
{
  public:
    ListItemService(IOService& ioService,
                    ConfigService& configService,
                    ListItemRepository& listItemRepository,
                    DescriptionRepository& descriptionRepository,
                    PriorityService& priorityService,
                    StatusService& statusService);
    PriorityService& priority();
    StatusService& status();
    std::vector<ListItemEntity> get(ListName& listName);
    ListItemEntity find(const std::string& id, ListName& listName);
    bool remove(const std::string& id, ListName& listName);
    std::string softDelete(const std::string& id, ListName& listName);
    std::string add(ListName& listName,
                    const std::string& itemValue,
                    const std::string* priority = nullptr,
                    const std::string* status = nullptr,
                    time_t dueAt = 0);
    const std::string makeId(ListName& listName);
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
    std::string duplicate(const std::string& id, ListName& listName);
    std::string archive(const std::string& id, ListName& listName);
    // Moves an item to the archive or delete variant of the same list (same name required). An active
    // (4-char) id gets a random [a-z] suffix, unique across archive + delete; archive <-> delete keeps
    // the 5-char id. Returns the new id.
    std::string moveToVariant(const std::string& id, ListName& listName, ListName& targetListName);
    // TEMPORARY: true when the list's archive / delete still hold pre-migration 4-char ids
    bool hasLegacyVariantIds(ListName& listName);
    // TEMPORARY: true when the list (or its archive / delete) still holds rows without parent_id
    bool hasLegacyRowLayout(ListName& listName);
    void archiveAll(ListName& listName);
    void archiveFinishedItems(ListName& listName, bool withDescribedItems);
    // Restores an archived / deleted item by its 5-char id. Returns the id it gets in the active
    // list: its original 4 chars when free, a new id otherwise.
    std::string restore(const std::string& id, ListName& listName);
    void editDeadline(std::string& id, ListName& listName, time_t dueAt = 0);
    long count(ListName& listName);
    long countWithStatus(ListName& listName, const std::vector<int>& status);
    long countWithPriority(ListName& listName, const std::vector<int>& priorities);
    long countCreatedBetween(ListName& listName, time_t from, time_t to);
    long countClosedBetween(ListName& listName, time_t from, time_t to);
    ListCountSummary getCountSummary(const std::vector<ListName>& listNames);
    void filterPriorityAbove(std::vector<ListItemEntity>& listItems, const int priority);
    void filterStatus(std::vector<ListItemEntity>& listItems, const std::vector<int>& statuses);
    void filterDeadlineBefore(std::vector<ListItemEntity>& listItems, const time_t timestamp);
    void filterDescribed(std::vector<ListItemEntity>& listItems);

  private:
    IOService& ioService;
    ConfigService& configService;
    ListItemRepository& listItemRepository;
    DescriptionRepository& descriptionRepository;
    PriorityService priorityService;
    StatusService statusService;
    std::vector<ListItemEntity> sort(std::vector<ListItemEntity> listItems);
    std::string makeVariantId(const std::string& baseId, ListName& listName);

  protected:
    void archiveItem(ListItemEntity& listItem, ListName& listName);
};

#endif // LISTITEMSERVICE_H
