#include "ListItemService.h"
#include "ListItemId.h"
#include "ListItems/ListItemEntity.h"

#include <algorithm>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>

ListItemService::ListItemService(IOService& ioService,
                                 ConfigService& configService,
                                 ListItemRepository& listItemRepository,
                                 DescriptionRepository& descriptionRepository,
                                 PriorityService& priorityService,
                                 StatusService& statusService)
  : ioService(ioService)
  , configService(configService)
  , listItemRepository(listItemRepository)
  , descriptionRepository(descriptionRepository)
  , priorityService(priorityService)
  , statusService(statusService)
{
}

std::vector<ListItemEntity>
ListItemService::get(ListName& listName)
{
    auto listItems = listItemRepository.get(listName);
    auto existingDescriptions = descriptionRepository.getIds(listName);

    for (auto& listItem : listItems) {
        bool hasDescription = false;
        if (std::count(existingDescriptions.begin(), existingDescriptions.end(), *listItem.getId()) > 0) {
            hasDescription = true;
        }

        listItem.setHasDescription(hasDescription);
    }

    return sort(listItems);
}

ListItemEntity
ListItemService::find(const std::string& id, ListName& listName)
{
    ListItemEntity listItem = listItemRepository.find(id, listName);
    listItem.setHasDescription(descriptionRepository.exists(id, listName));

    return listItem;
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
                     time_t dueAt,
                     const std::string& parentId,
                     const std::string& requestedId)
{
    if (itemValue.length() > 255) {
        throw std::invalid_argument("Item value must not exceed 255 characters.");
    }

    if (!parentId.empty()) {
        if (listName.getVariant() != "default") {
            throw std::invalid_argument("Child items can only be added to the active list.");
        }
        validateParent("", parentId, listName);
    }

    // A requested id is used when it is free, a generated one otherwise (the caller compares them)
    if (!requestedId.empty()) {
        if (listName.getVariant() != "default") {
            throw std::invalid_argument("An id can only be chosen in the active list.");
        }
        if (!ListItemId::isValidActive(requestedId)) {
            throw std::invalid_argument("The id must be 4 letters or digits.");
        }
    }
    std::string id = !requestedId.empty() && isIdAvailable(requestedId, listName) ? requestedId : makeId(listName);

    ListItemEntity listItemEntity(listName);
    listItemEntity.setId(id);

    if (priority != nullptr) {
        PriorityEntity priorityEntity = priorityService.getPriorityFromName(*priority);
        listItemEntity.setPriority(priorityEntity);
    } else {
        PriorityEntity priorityEntity = priorityService.getPriorityFromName("urgent");
        listItemEntity.setPriority(priorityEntity);
    }
    if (status != nullptr) {
        StatusEntity statusEntity = statusService.getStatusFromName(*status);
        listItemEntity.setStatus(statusEntity);
    } else {
        StatusEntity statusEntity = statusService.getStatusFromName("queued");
        listItemEntity.setStatus(statusEntity);
    }

    listItemEntity.setValue(itemValue);
    listItemEntity.setCreatedAt(time(nullptr));
    listItemEntity.setUpdatedAt(time(nullptr));
    listItemEntity.setDueAt(dueAt);
    listItemEntity.setParentId(parentId);
    listItemRepository.create(listItemEntity, listName);

    return id;
}

std::string const
ListItemService::makeId(ListName& listName)
{
    bool validId = false;
    int i = 0;
    std::string id;
    const std::string generationType = configService.getValue("idRandomGenerationType");
    while (!validId && i < 50) {
        id = ListItemId::generate(generationType);
        if (isIdAvailable(id, listName)) {
            validId = true;
        }

        i++;
    }

    if (!validId) {
        throw std::runtime_error("Could not generate a free ID for list " + listName.getName() + ".");
    }

    return id;
}

bool
ListItemService::isIdAvailable(const std::string& id, ListName& listName)
{
    // Archived / deleted items carry a 5-char id, so only the active list can hold a 4-char id
    for (const ListItemEntity& item : listItemRepository.get(listName)) {
        if (*item.getId() == id)
            return false;
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

    applyStatus(id, listItemToUpdate, listName, statusService.find(*status));
}

void
ListItemService::applyStatus(const std::string& id,
                             ListItemEntity& listItemToUpdate,
                             ListName& listName,
                             const StatusEntity& statusEntity)
{
    const bool isClosed = statusService.isClosed(*statusEntity.getName());

    listItemToUpdate.setStatus(statusEntity);
    listItemToUpdate.setUpdatedAt(time(nullptr));
    listItemToUpdate.setClosedAt(isClosed ? time(nullptr) : 0);

    listItemRepository.update(id, listName, listItemToUpdate);

    if (isClosed && configService.isTrue("archiveWhenCompleted") && listName.getVariant() == "default") {
        autoArchive(id, listName);
    }
}

void
ListItemService::reset(const std::string& id, ListName& listName)
{
    ListItemEntity listItemToUpdate(listName);
    listItemToUpdate = find(id, listName);
    StatusEntity statusEntity = statusService.getStatusFromName("queued");
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
    // Items without a parent first: their children follow them. Then whatever is left (children
    // whose parent is not in the list).
    for (ListItemEntity& listItem : listItemRepository.get(listName)) {
        if (!listItem.hasParent()) {
            archiveItem(listItem, listName);
        }
    }
    for (ListItemEntity& listItem : listItemRepository.get(listName)) {
        archiveItem(listItem, listName);
    }
}

void
ListItemService::archiveFinishedItems(ListName& listName, bool withDescribedItems, bool withAttachedItems)
{
    auto isArchivable = [withDescribedItems](const ListItemEntity& item) {
        return *(*item.status()).isClosed() && (withDescribedItems || !*item.hasDescription());
    };

    // Top-level items. A parent takes its children along, so it only goes when all of them can go too.
    // Parent / child relations are read from this snapshot: archiving a parent removes its children.
    std::vector<ListItemEntity> listItems = get(listName);
    auto isChild = [&listItems](const ListItemEntity& item) {
        return item.hasParent() && std::any_of(listItems.begin(), listItems.end(), [&item](const ListItemEntity& i) {
                   return *i.getId() == *item.getParentId();
               });
    };
    for (ListItemEntity& listItem : listItems) {
        if (isChild(listItem) || !isArchivable(listItem)) {
            continue;
        }

        std::vector<ListItemEntity> children;
        std::copy_if(listItems.begin(), listItems.end(), std::back_inserter(children), [&](const ListItemEntity& i) {
            return *i.getParentId() == *listItem.getId();
        });
        if (!children.empty() && (!withAttachedItems || !std::all_of(children.begin(), children.end(), isArchivable))) {
            continue;
        }

        archiveItem(listItem, listName);
    }

    if (!withAttachedItems) {
        return;
    }

    // Finished children whose parent stays in the list
    for (ListItemEntity& listItem : get(listName)) {
        if (hasParentInList(listItem, listName) && isArchivable(listItem)) {
            archiveItem(listItem, listName);
        }
    }
}

void
ListItemService::autoArchive(const std::string& id, ListName& listName)
{
    ListItemEntity listItem = listItemRepository.find(id, listName);

    // Children are archived together with their parent: once the parent is finished and this was
    // its last open child
    if (hasParentInList(listItem, listName)) {
        const std::string& parentId = *listItem.getParentId();
        ListItemEntity parent = listItemRepository.find(parentId, listName);
        if (*(*parent.status()).isClosed() && !hasOpenChild(parentId, listName)) {
            archive(parentId, listName);
        }
        return;
    }

    // A parent waits for its open children
    if (!hasOpenChild(id, listName)) {
        archive(id, listName);
    }
}

void
ListItemService::archiveItem(ListItemEntity& listItem, ListName& listName)
{
    ListName listNameArchive = ListName::createVariant(listName, "archive");
    moveToVariant(*listItem.getId(), listName, listNameArchive);
}

std::string
ListItemService::softDelete(const std::string& id, ListName& listName)
{
    ListName listNameDelete = ListName::createVariant(listName, "delete");
    return moveToVariant(id, listName, listNameDelete);
}

std::string
ListItemService::moveToVariant(const std::string& id, ListName& listName, ListName& targetListName)
{
    if (listName.getName() != targetListName.getName()) {
        throw std::invalid_argument("Items can only be moved to the archive or delete list of their own list.");
    }
    if (targetListName.getVariant() == "default") {
        throw std::invalid_argument("Use restore to bring an item back to the active list.");
    }
    if (listName.getVariant() == targetListName.getVariant()) {
        throw std::invalid_argument("Item " + id + " is already in the " + targetListName.getVariant() + " list.");
    }

    ListItemEntity listItem = listItemRepository.find(id, listName);
    std::vector<ListItemEntity> children = getChildren(id, listName);

    // Coming from archive <-> delete: the 5-char id is already unique across both lists, keep it
    const bool fromActive = listName.getVariant() == "default";
    std::string newId = fromActive ? makeVariantId(id, listName) : id;

    // A child going on its own keeps its parent id, so that a restore can attach it back
    listItem.setId(newId);
    listItemRepository.create(listItem, targetListName);
    listItemRepository.remove(id, listName);
    descriptionRepository.rename(id, newId, listName);

    // Children follow their parent
    for (ListItemEntity& child : children) {
        std::string childId = *child.getId();
        std::string newChildId = fromActive ? makeVariantId(childId, listName) : childId;
        child.setId(newChildId);
        child.setParentId(newId);
        listItemRepository.create(child, targetListName);
        listItemRepository.remove(childId, listName);
        descriptionRepository.rename(childId, newChildId, listName);
    }

    // Children archived / deleted earlier on their own follow the parent's new id
    if (fromActive) {
        relinkVariantChildren(id, newId, listName);
    }

    return newId;
}

void
ListItemService::relinkVariantChildren(const std::string& oldParentId,
                                       const std::string& newParentId,
                                       ListName& listName)
{
    for (const std::string variant : { "archive", "delete" }) {
        ListName variantListName = ListName::createVariant(listName, variant);
        for (ListItemEntity& item : listItemRepository.get(variantListName)) {
            if (*item.getParentId() == oldParentId) {
                item.setParentId(newParentId);
                listItemRepository.update(*item.getId(), variantListName, item);
            }
        }
    }
}

// TEMPORARY (remove once every install is migrated): data written before 5-char archive/delete ids
bool
ListItemService::hasLegacyVariantIds(ListName& listName)
{
    for (const std::string variant : { "archive", "delete" }) {
        ListName variantListName = ListName::createVariant(listName, variant);
        for (const ListItemEntity& item : listItemRepository.get(variantListName)) {
            if ((*item.getId()).length() == static_cast<size_t>(ListItemId::getIdLength())) {
                return true;
            }
        }
    }

    return false;
}

// TEMPORARY (remove once every install is migrated): rows written before the parent_id column
bool
ListItemService::hasLegacyRowLayout(ListName& listName)
{
    for (const std::string variant : { "default", "archive", "delete" }) {
        ListName variantListName = ListName::createVariant(listName, variant);
        if (listItemRepository.hasLegacyRows(variantListName)) {
            return true;
        }
    }

    return false;
}

std::string
ListItemService::makeVariantId(const std::string& baseId, ListName& listName)
{
    // Archive and delete lists share the description directory and are both searched by restore:
    // the suffixed id must be unique across both of them.
    ListName listNameArchive = ListName::createVariant(listName, "archive");
    ListName listNameDelete = ListName::createVariant(listName, "delete");
    std::vector<std::string> takenIds;
    for (const ListItemEntity& item : listItemRepository.get(listNameArchive)) {
        takenIds.push_back(*item.getId());
    }
    for (const ListItemEntity& item : listItemRepository.get(listNameDelete)) {
        takenIds.push_back(*item.getId());
    }

    // Random starting letter, then walk the alphabet: random suffix, guaranteed to end
    const std::string letters = "abcdefghijklmnopqrstuvwxyz";
    const size_t start = letters.find(StringHelpers::randomLettersLowercase(1)[0]);
    for (size_t i = 0; i < letters.size(); i++) {
        std::string candidate = ListItemId::withSuffix(baseId, letters[(start + i) % letters.size()]);
        if (std::find(takenIds.begin(), takenIds.end(), candidate) == takenIds.end()) {
            return candidate;
        }
    }

    throw std::runtime_error("No free archive/delete ID left for base " + baseId + ".");
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

    applyStatus(id, listItemToUpdate, listName, statusService.getStatusFromName(*statusName));
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

std::vector<std::pair<std::string, std::string>>
ListItemService::move(const std::string& id, ListName& oldListName, ListName& newListName)
{
    if (oldListName.getVariant() != "default") {
        throw std::invalid_argument("Archived or deleted items cannot be moved. Restore them first.");
    }
    if (oldListName.getName() == newListName.getName()) {
        throw std::invalid_argument("Cannot move item to the same list.");
    }

    ListItemEntity listItemToMove = listItemRepository.find(id, oldListName);
    std::vector<ListItemEntity> children = getChildren(id, oldListName);

    // An existing item with the same id in the new list is overwritten
    clearTarget(id, newListName);

    // A child moved on its own becomes top-level
    listItemToMove.setParentId("");
    listItemRepository.create(listItemToMove, newListName);
    listItemRepository.remove(id, oldListName);
    descriptionRepository.move(id, oldListName, id, newListName);
    // The parent left this list: its archived / deleted children lose their link
    relinkVariantChildren(id, "", oldListName);

    std::vector<std::pair<std::string, std::string>> movedChildren;
    for (ListItemEntity& child : children) {
        std::string childId = *child.getId();
        std::string newChildId = isIdAvailable(childId, newListName) ? childId : makeId(newListName);
        clearTarget(newChildId, newListName);
        child.setId(newChildId);
        listItemRepository.create(child, newListName);
        listItemRepository.remove(childId, oldListName);
        descriptionRepository.move(childId, oldListName, newChildId, newListName);
        movedChildren.emplace_back(childId, newChildId);
    }

    return movedChildren;
}

std::vector<std::pair<std::string, std::string>>
ListItemService::copy(const std::string& id, ListName& oldListName, ListName& newListName)
{
    if (oldListName.getVariant() != "default") {
        throw std::invalid_argument("Archived or deleted items cannot be copied. Restore them first.");
    }
    if (oldListName.getName() == newListName.getName()) {
        throw std::invalid_argument("Cannot copy item to the same list.");
    }

    ListItemEntity listItemToCopy = listItemRepository.find(id, oldListName);

    // An existing item with the same id in the new list is overwritten
    clearTarget(id, newListName);

    // A child copied on its own becomes top-level
    listItemToCopy.setParentId("");
    listItemRepository.create(listItemToCopy, newListName);
    descriptionRepository.copy(id, oldListName, id, newListName);

    std::vector<std::pair<std::string, std::string>> copiedChildren;

    for (ListItemEntity& child : getChildren(id, oldListName)) {
        std::string childId = *child.getId();
        std::string newChildId = isIdAvailable(childId, newListName) ? childId : makeId(newListName);
        clearTarget(newChildId, newListName);
        child.setId(newChildId);
        listItemRepository.create(child, newListName);
        descriptionRepository.copy(childId, oldListName, newChildId, newListName);
        copiedChildren.emplace_back(childId, newChildId);
    }

    return copiedChildren;
}

void
ListItemService::clearTarget(const std::string& id, ListName& listName)
{
    // also drops an orphan description left under a free id, so the new item does not inherit it
    descriptionRepository.remove(id, listName);
    if (!exists(id, listName)) {
        return;
    }

    detachChildren(id, listName);
    listItemRepository.remove(id, listName);
}

std::string
ListItemService::duplicate(const std::string& id, ListName& listName)
{
    if (listName.getVariant() != "default") {
        throw std::invalid_argument("Archived or deleted items cannot be duplicated. Restore them first.");
    }

    ListItemEntity listItemToDuplicate = listItemRepository.find(id, listName);
    std::string newId = makeId(listName);
    clearTarget(newId, listName);
    listItemToDuplicate.setId(newId);
    listItemRepository.create(listItemToDuplicate, listName);
    descriptionRepository.copy(id, listName, newId, listName);

    return newId;
}

void
ListItemService::reassign(const std::string& id, const std::string& newId, ListName& listName)
{
    if (listName.getVariant() != "default") {
        throw std::invalid_argument("Only active items can be reassigned. Restore them first.");
    }
    if (!ListItemId::isValidActive(newId)) {
        throw std::invalid_argument("The new id must be 4 letters or digits.");
    }

    ListItemEntity listItem = listItemRepository.find(id, listName);
    if (newId == id) {
        throw std::invalid_argument("Item " + id + " already has this id.");
    }
    if (!isIdAvailable(newId, listName)) {
        throw std::invalid_argument("Id " + newId + " is already used in list " + listName.getName() + ".");
    }

    // Updated in place: the item keeps its position in the file
    listItem.setId(newId);
    listItem.setUpdatedAt(time(nullptr));
    listItemRepository.update(id, listName, listItem);

    for (ListItemEntity& child : getChildren(id, listName)) {
        child.setParentId(newId);
        listItemRepository.update(*child.getId(), listName, child);
    }
    relinkVariantChildren(id, newId, listName);
    descriptionRepository.rename(id, newId, listName);
}

std::string
ListItemService::restore(const std::string& id, ListName& listName)
{
    ListName listNameArchive = ListName::createVariant(listName, "archive");
    ListName listNameDelete = ListName::createVariant(listName, "delete");

    ListItemEntity listItem(listName);
    ListName sourceListName = listNameArchive;
    bool found = false;
    for (ListName& variantListName : std::vector<ListName>{ listNameArchive, listNameDelete }) {
        try {
            listItem = listItemRepository.find(id, variantListName);
            sourceListName = variantListName;
            found = true;
            break;
        } catch (ListItemNotFoundException& e) {
            // not in this variant
        }
    }

    if (!found) {
        throw std::invalid_argument("Item to restore couldn't be found.");
    }

    std::string oldId = *listItem.getId();
    std::vector<ListItemEntity> children = getChildren(oldId, sourceListName);

    // Give back the original 4-char id when it is still free in the active list
    std::string baseId = ListItemId::baseOf(oldId);
    std::string newId = isIdAvailable(baseId, listName) ? baseId : makeId(listName);

    // A child restored on its own stays attached only if its parent is active (and top-level)
    if (listItem.hasParent()) {
        try {
            if (listItemRepository.find(*listItem.getParentId(), listName).hasParent()) {
                listItem.setParentId("");
            }
        } catch (ListItemNotFoundException& e) {
            listItem.setParentId("");
        }
    }

    listItem.setId(newId);
    listItemRepository.create(listItem, listName);
    listItemRepository.remove(oldId, sourceListName);
    descriptionRepository.rename(oldId, newId, listName);

    // Children come back with their parent
    for (ListItemEntity& child : children) {
        std::string childId = *child.getId();
        std::string childBaseId = ListItemId::baseOf(childId);
        std::string newChildId = isIdAvailable(childBaseId, listName) ? childBaseId : makeId(listName);
        child.setId(newChildId);
        child.setParentId(newId);
        listItemRepository.create(child, listName);
        listItemRepository.remove(childId, sourceListName);
        descriptionRepository.rename(childId, newChildId, listName);
    }

    // Children left in the archive / delete lists now point to the active parent
    relinkVariantChildren(oldId, newId, listName);

    return newId;
}

std::string
ListItemService::archive(const std::string& id, ListName& listName)
{
    ListName listNameArchive = ListName::createVariant(listName, "archive");
    return moveToVariant(id, listName, listNameArchive);
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
    for (int statusId : status) {
        if (!statusService.isIdValid(statusId)) {
            throw std::invalid_argument("Status does not exists.");
        }
    }

    long count = 0;
    const std::vector<ListItemEntity> listItems = listItemRepository.get(listName);
    for (const ListItemEntity& listItem : listItems) {
        for (int statusId : status) {
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
    for (int priorityId : priorities) {
        if (!priorityService.isIdValid(priorityId)) {
            throw std::invalid_argument("Priority does not exists.");
        }
    }

    long count = 0;
    const std::vector<ListItemEntity> listItems = listItemRepository.get(listName);
    for (const ListItemEntity& listItem : listItems) {
        for (int priorityId : priorities) {
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

ListCountSummary
ListItemService::getCountSummary(const std::vector<ListName>& listNames)
{
    ListCountSummary summary;
    for (auto listName : listNames) {
        // Active items — one pass for total, statuses, priorities, delivered, cancelled, lastUpdate
        for (const auto& item : listItemRepository.get(listName)) {
            summary.total++;
            int statusId = *(*item.status()).getId();
            int priorityId = *(*item.priority()).getId();
            summary.addStatus(statusId);
            summary.addPriority(priorityId);
            if (statusId == StatusService::COMPLETED)
                summary.delivered++;
            if (statusId == StatusService::CANCELLED)
                summary.cancelled++;
            if (*item.getUpdatedAt() > summary.lastUpdate)
                summary.lastUpdate = *item.getUpdatedAt();
        }
        // Archive items — one pass for archived count, delivered, cancelled, lastUpdate
        ListName archiveName = ListName::createVariant(listName, "archive");
        for (const auto& item : listItemRepository.get(archiveName)) {
            summary.archived++;
            int statusId = *(*item.status()).getId();
            if (statusId == StatusService::COMPLETED)
                summary.delivered++;
            if (statusId == StatusService::CANCELLED)
                summary.cancelled++;
            if (*item.getUpdatedAt() > summary.lastUpdate)
                summary.lastUpdate = *item.getUpdatedAt();
        }
        // Deleted items — count all, track lastUpdate
        ListName deleteName = ListName::createVariant(listName, "delete");
        for (const auto& item : listItemRepository.get(deleteName)) {
            summary.deleted++;
            if (*item.getUpdatedAt() > summary.lastUpdate)
                summary.lastUpdate = *item.getUpdatedAt();
        }
    }
    return summary;
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

std::vector<ListItemEntity>
ListItemService::getChildren(const std::string& parentId, ListName& listName)
{
    std::vector<ListItemEntity> children;
    for (const ListItemEntity& item : listItemRepository.get(listName)) {
        if (*item.getParentId() == parentId) {
            children.push_back(item);
        }
    }

    return children;
}

std::vector<std::string>
ListItemService::getChildIdsFollowingStatus(const std::string& parentId,
                                            ListName& listName,
                                            const StatusEntity& targetStatus)
{
    find(parentId, listName); // throws when the parent does not exist

    std::vector<std::string> childIds;
    for (const ListItemEntity& child : getChildren(parentId, listName)) {
        const StatusEntity* childStatus = child.status();
        if (!*childStatus->isClosed() && *childStatus->getId() != *targetStatus.getId()) {
            childIds.push_back(*child.getId());
        }
    }

    return childIds;
}

bool
ListItemService::isParent(const std::string& id, ListName& listName)
{
    const std::vector<ListItemEntity> listItems = listItemRepository.get(listName);
    return std::any_of(
        listItems.begin(), listItems.end(), [&id](const ListItemEntity& item) { return *item.getParentId() == id; });
}

bool
ListItemService::exists(const std::string& id, ListName& listName)
{
    const std::vector<ListItemEntity> listItems = listItemRepository.get(listName);
    return std::any_of(
        listItems.begin(), listItems.end(), [&id](const ListItemEntity& item) { return *item.getId() == id; });
}

bool
ListItemService::hasParentInList(const ListItemEntity& item, ListName& listName)
{
    return item.hasParent() && exists(*item.getParentId(), listName);
}

bool
ListItemService::hasOpenChild(const std::string& parentId, ListName& listName)
{
    const std::vector<ListItemEntity> children = getChildren(parentId, listName);
    return std::any_of(
        children.begin(), children.end(), [](const ListItemEntity& child) { return !*(*child.status()).isClosed(); });
}

void
ListItemService::validateParent(const std::string& id, const std::string& parentId, ListName& listName)
{
    if (id == parentId) {
        throw std::invalid_argument("An item cannot be attached to itself.");
    }

    ListItemEntity parent(listName);
    try {
        parent = listItemRepository.find(parentId, listName);
    } catch (ListItemNotFoundException& e) {
        throw std::invalid_argument("Parent " + parentId + " not found in list " + listName.getName() + ".");
    }

    if (parent.hasParent()) {
        throw std::invalid_argument(parentId + " is itself a child item: children cannot have children.");
    }
    if (!id.empty() && isParent(id, listName)) {
        throw std::invalid_argument(id + " has children: it cannot become a child.");
    }
}

void
ListItemService::attach(const std::string& id, const std::string& parentId, ListName& listName, bool force)
{
    if (listName.getVariant() != "default") {
        throw std::invalid_argument("Only active items can be attached. Restore them first.");
    }

    ListItemEntity listItem = listItemRepository.find(id, listName);
    validateParent(id, parentId, listName);

    if (*listItem.getParentId() == parentId) {
        throw std::invalid_argument("Item " + id + " is already attached to " + parentId + ".");
    }
    if (listItem.hasParent() && !force) {
        throw std::invalid_argument("Item " + id + " is already attached to " + *listItem.getParentId() +
                                    ": use --force to attach it to " + parentId + ".");
    }

    listItem.setParentId(parentId);
    listItem.setUpdatedAt(time(nullptr));
    listItemRepository.update(id, listName, listItem);
}

void
ListItemService::detach(const std::string& id, ListName& listName)
{
    ListItemEntity listItem = listItemRepository.find(id, listName);
    if (!listItem.hasParent()) {
        throw std::invalid_argument("Item " + id + " is not attached to a parent.");
    }

    listItem.setParentId("");
    listItem.setUpdatedAt(time(nullptr));
    listItemRepository.update(id, listName, listItem);
}

long
ListItemService::detachChildren(const std::string& parentId, ListName& listName)
{
    long count = 0;
    for (ListItemEntity& child : getChildren(parentId, listName)) {
        child.setParentId("");
        child.setUpdatedAt(time(nullptr));
        listItemRepository.update(*child.getId(), listName, child);
        count++;
    }
    // "Detach all" also covers the children archived / deleted on their own
    relinkVariantChildren(parentId, "", listName);

    return count;
}

std::vector<std::string>
ListItemService::withoutChildrenOfListedParents(const std::vector<std::string>& ids, ListName& listName)
{
    std::vector<std::string> filteredIds;
    for (const std::string& id : ids) {
        try {
            ListItemEntity listItem = listItemRepository.find(id, listName);
            if (listItem.hasParent() && std::find(ids.begin(), ids.end(), *listItem.getParentId()) != ids.end()) {
                continue;
            }
        } catch (ListItemNotFoundException& e) {
            // unknown ids are kept: the action reports them
        }
        filteredIds.push_back(id);
    }

    return filteredIds;
}

std::vector<std::string>
ListItemService::removePermanently(const std::string& id, ListName& listName)
{
    listItemRepository.find(id, listName); // throws when the item does not exist

    std::vector<std::string> removedChildIds;
    for (const ListItemEntity& child : getChildren(id, listName)) {
        listItemRepository.remove(*child.getId(), listName);
        descriptionRepository.remove(*child.getId(), listName);
        removedChildIds.push_back(*child.getId());
    }

    listItemRepository.remove(id, listName);
    descriptionRepository.remove(id, listName);
    relinkVariantChildren(id, "", listName);

    return removedChildIds;
}

std::string
ListItemService::itemKey(const ListItemEntity& item, const std::string& id)
{
    return (*item.getListName()).getName() + ":" + (*item.getListName()).getVariant() + ":" + id;
}

void
ListItemService::groupChildren(std::vector<ListItemEntity>& listItems)
{
    std::unordered_map<std::string, const ListItemEntity*> itemsByKey;
    for (const ListItemEntity& item : listItems) {
        itemsByKey[itemKey(item, *item.getId())] = &item;
    }

    // A row is shown as a child when its parent is shown as a top-level row (one level only)
    auto shownParentKey = [&itemsByKey](const ListItemEntity& item) -> std::string {
        if (!item.hasParent()) {
            return "";
        }
        auto parent = itemsByKey.find(itemKey(item, *item.getParentId()));
        if (parent == itemsByKey.end() || parent->second->hasParent()) {
            return "";
        }
        return parent->first;
    };

    std::vector<const ListItemEntity*> roots;
    std::unordered_map<std::string, std::vector<const ListItemEntity*>> childrenByParent;
    for (const ListItemEntity& item : listItems) {
        std::string parentKey = shownParentKey(item);
        if (parentKey.empty()) {
            roots.push_back(&item);
        } else {
            childrenByParent[parentKey].push_back(&item);
        }
    }

    std::vector<ListItemEntity> grouped;
    grouped.reserve(listItems.size());
    for (const ListItemEntity* root : roots) {
        grouped.push_back(*root);
        auto children = childrenByParent.find(itemKey(*root, *root->getId()));
        if (children != childrenByParent.end()) {
            for (const ListItemEntity* child : children->second) {
                grouped.push_back(*child);
            }
        }
    }

    listItems = std::move(grouped);
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

void
ListItemService::filterDescribed(std::vector<ListItemEntity>& listItems)
{
    listItems.erase(std::remove_if(listItems.begin(),
                                   listItems.end(),
                                   [](const ListItemEntity& item) { return !(*item.hasDescription()); }),
                    listItems.end());
}
