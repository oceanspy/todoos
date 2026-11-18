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
                    time_t dueAt = 0,
                    const std::string& parentId = "");
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
    // Moves / copies an item to another list. A parent takes its children along (a child keeps its id
    // when free in the target list, gets a new one otherwise); a child on its own becomes top-level.
    // Returns the (old id, new id) of the children that followed.
    std::vector<std::pair<std::string, std::string>> move(const std::string& id,
                                                          ListName& oldListName,
                                                          ListName& newListName);
    std::vector<std::pair<std::string, std::string>> copy(const std::string& id,
                                                          ListName& oldListName,
                                                          ListName& newListName);
    std::string duplicate(const std::string& id, ListName& listName);
    std::string archive(const std::string& id, ListName& listName);
    // Moves an item to the archive or delete variant of the same list (same name required). An active
    // (4-char) id gets a random [a-z] suffix, unique across archive + delete; archive <-> delete keeps
    // the 5-char id. Its children follow it, attached to its new id. Returns the new id.
    std::string moveToVariant(const std::string& id, ListName& listName, ListName& targetListName);
    // TEMPORARY: true when the list's archive / delete still hold pre-migration 4-char ids
    bool hasLegacyVariantIds(ListName& listName);
    // TEMPORARY: true when the list (or its archive / delete) still holds rows without parent_id
    bool hasLegacyRowLayout(ListName& listName);
    void archiveAll(ListName& listName);
    // Archives completed / cancelled items. Children and parents that still have children are kept,
    // unless withAttachedItems: then finished children are archived too, and a finished parent goes
    // (with its children) once none of them is open.
    void archiveFinishedItems(ListName& listName, bool withDescribedItems, bool withAttachedItems = false);
    // Restores an archived / deleted item by its 5-char id. Returns the id it gets in the active
    // list: its original 4 chars when free, a new id otherwise. Its children are restored with it;
    // a child restored on its own stays attached only if its parent is active.
    std::string restore(const std::string& id, ListName& listName);
    void editDeadline(std::string& id, ListName& listName, time_t dueAt = 0);
    long count(ListName& listName);
    long countWithStatus(ListName& listName, const std::vector<int>& status);
    long countWithPriority(ListName& listName, const std::vector<int>& priorities);
    long countCreatedBetween(ListName& listName, time_t from, time_t to);
    long countClosedBetween(ListName& listName, time_t from, time_t to);
    ListCountSummary getCountSummary(const std::vector<ListName>& listNames);
    // ---- parent / child items (one level: a child cannot have children) ----
    std::vector<ListItemEntity> getChildren(const std::string& parentId, ListName& listName);
    bool isParent(const std::string& id, ListName& listName);
    // An item already attached to another parent is only re-attached with force
    void attach(const std::string& id, const std::string& parentId, ListName& listName, bool force = false);
    void detach(const std::string& id, ListName& listName);
    // Detaches every child of the parent. Returns how many were detached.
    long detachChildren(const std::string& parentId, ListName& listName);
    // Drops the ids whose parent is also listed: they follow their parent anyway
    std::vector<std::string> withoutChildrenOfListedParents(const std::vector<std::string>& ids, ListName& listName);
    // Permanently removes an item, its children and their descriptions. Returns the removed child ids.
    std::vector<std::string> removePermanently(const std::string& id, ListName& listName);
    // Puts each child right after its parent (children keep their relative order). Children whose
    // parent is not in the vector stay where they are, as top-level items.
    static void groupChildren(std::vector<ListItemEntity>& listItems);
    // "list:variant:id": ids are only unique within a list
    static std::string itemKey(const ListItemEntity& item, const std::string& id);
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
    bool exists(const std::string& id, ListName& listName);
    // True when the item's parent is in the same list
    bool hasParentInList(const ListItemEntity& item, ListName& listName);
    bool hasOpenChild(const std::string& parentId, ListName& listName);
    void validateParent(const std::string& id, const std::string& parentId, ListName& listName);
    void autoArchive(const std::string& id, ListName& listName);
    // Removes the item `id` of the target list before it is overwritten, detaching its children
    void clearTarget(const std::string& id, ListName& listName);

  protected:
    void archiveItem(ListItemEntity& listItem, ListName& listName);
};

#endif // LISTITEMSERVICE_H
