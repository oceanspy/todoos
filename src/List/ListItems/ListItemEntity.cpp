#include "ListItemEntity.h"
#include "../ListItemId.h"
#include <stdexcept>
#include <utility>

ListItemEntity::ListItemEntity(ListName& listName)
  : listName(listName)
{
}

const std::string*
ListItemEntity::getId() const
{
    return &id;
}

const std::string*
ListItemEntity::getParentId() const
{
    return &parentId;
}

bool
ListItemEntity::hasParent() const
{
    return !parentId.empty();
}

const int*
ListItemEntity::getPosition() const
{
    return &position;
}

const ListName*
ListItemEntity::getListName() const
{
    return &listName;
}

const std::string*
ListItemEntity::getValue() const
{
    return &value;
}

const PriorityEntity*
ListItemEntity::priority() const
{
    return &priorityEntity;
}

const StatusEntity*
ListItemEntity::status() const
{
    return &statusEntity;
}

const bool*
ListItemEntity::hasDescription() const
{
    return &descriptionExists;
}

const time_t*
ListItemEntity::getDueAt() const
{
    return &dueAt;
}

const time_t*
ListItemEntity::getClosedAt() const
{
    return &closedAt;
}

const time_t*
ListItemEntity::getCreatedAt() const
{
    return &createdAt;
}

const time_t*
ListItemEntity::getUpdatedAt() const
{
    return &updatedAt;
}

void
ListItemEntity::setId(std::string idStr)
{
    // 4 alphanumeric chars for active items, 5 for archived / deleted items
    if (!ListItemId::isValid(idStr)) {
        throw std::invalid_argument("Invalid ID: it must be 4 or 5 alphanumeric characters long");
    }

    this->id = idStr;
}

void
ListItemEntity::setParentId(const std::string& parentIdStr)
{
    if (!parentIdStr.empty() && !ListItemId::isValid(parentIdStr)) {
        throw std::invalid_argument("Invalid parent ID: it must be empty or 4 or 5 alphanumeric characters long");
    }

    this->parentId = parentIdStr;
}

void
ListItemEntity::setPosition(int positionInt)
{
    if (positionInt < 0) {
        throw std::invalid_argument("Position cannot be negative");
    }

    this->position = positionInt;
}

void
ListItemEntity::setValue(const std::string& valueStr)
{
    if (valueStr.empty()) {
        throw std::invalid_argument("Value cannot be empty");
    }

    this->value = valueStr;
}

void
ListItemEntity::setPriority(const PriorityEntity& priorityEntity)
{
    this->priorityEntity = priorityEntity;
}

void
ListItemEntity::setStatus(const StatusEntity& statusEntity)
{
    this->statusEntity = statusEntity;
}

void
ListItemEntity::setHasDescription(const bool& hasDescription)
{
    this->descriptionExists = hasDescription;
}

void
ListItemEntity::setDueAt(time_t dueAtDate)
{
    // if not timestamp, throw exception
    if (dueAtDate < 0) {
        throw std::invalid_argument("Created at timestamp cannot be negative");
    }

    this->dueAt = dueAtDate;
}

void
ListItemEntity::setClosedAt(time_t closedAtDate)
{
    // if not timestamp, throw exception
    if (closedAtDate < 0) {
        throw std::invalid_argument("Created at timestamp cannot be negative");
    }

    this->closedAt = closedAtDate;
}

void
ListItemEntity::setCreatedAt(time_t createdAtDate)
{
    // if not timestamp, throw exception
    if (createdAtDate < 0) {
        throw std::invalid_argument("Created at timestamp cannot be negative");
    }

    this->createdAt = createdAtDate;
}

void
ListItemEntity::setUpdatedAt(time_t updatedAtDate)
{
    // if not timestamp, throw exception
    if (updatedAtDate < 0) {
        throw std::invalid_argument("Created at timestamp cannot be negative");
    }

    this->updatedAt = updatedAtDate;
}

ListItemEntity
ListItemEntity::set(const std::string& id,
                    const std::string& value,
                    const PriorityEntity& priority,
                    const StatusEntity& status,
                    const time_t& dueAt,
                    const time_t& closedAt,
                    const time_t& createdAt,
                    const time_t& updatedAt,
                    ListName& listName)
{
    ListItemEntity listItemEntity(listName);
    listItemEntity.setId(id);
    listItemEntity.setValue(value);
    listItemEntity.setPriority(priority);
    listItemEntity.setStatus(status);
    listItemEntity.setDueAt(dueAt);
    listItemEntity.setClosedAt(closedAt);
    listItemEntity.setCreatedAt(createdAt);
    listItemEntity.setUpdatedAt(updatedAt);
    return listItemEntity;
}

ListItemEntity
ListItemEntity::setFromVector(PriorityService& priorityService,
                              StatusService& statusService,
                              const std::vector<std::string>& item,
                              ListName& listName)
{
    if (item.size() != ROW_SIZE) {
        throw std::invalid_argument("Invalid item: expected " + std::to_string(ROW_SIZE) + " columns");
    }

    ListItemEntity listItemEntity(listName);
    listItemEntity.setId(item[0]);
    listItemEntity.setParentId(item[1]);
    listItemEntity.setValue(item[2]);
    listItemEntity.setPriority(priorityService.getPriorityFromName(item[3]));
    listItemEntity.setStatus(statusService.getStatusFromName(item[4]));
    listItemEntity.setDueAt(std::stoi(item[5]));
    listItemEntity.setClosedAt(std::stoi(item[6]));
    listItemEntity.setCreatedAt(std::stoi(item[7]));
    listItemEntity.setUpdatedAt(std::stoi(item[8]));
    return listItemEntity;
}

std::vector<std::string>
ListItemEntity::makeVector(const ListItemEntity& listItemEntity)
{
    return { *listItemEntity.getId(),
             *listItemEntity.getParentId(),
             *listItemEntity.getValue(),
             *(*listItemEntity.priority()).getName(),
             *(*listItemEntity.status()).getCommandName(),
             std::to_string(*listItemEntity.getDueAt()),
             std::to_string(*listItemEntity.getClosedAt()),
             std::to_string(*listItemEntity.getCreatedAt()),
             std::to_string(*listItemEntity.getUpdatedAt()) };
}
