#include "ListItemEntity.h"
#include <stdexcept>
#include <utility>

const std::string*
ListItemEntity::getId() const
{
    return &id;
}

const int*
ListItemEntity::getPosition() const
{
    return &position;
}

const std::string*
ListItemEntity::getListName() const
{
    return &listName;
}

const std::string*
ListItemEntity::getListVariant() const
{
    return &listVariant;
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
    // ID cannot be empty and needs to be numbers and letters
    if (idStr.length() != 4) {
        throw std::invalid_argument("ID must be 4 characters long");
    }

    for (char ch : idStr) {
        if (!std::isalnum(ch)) {
            throw std::invalid_argument("ID must be alphanumeric");
        }
    }

    this->id = idStr;
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
ListItemEntity::setListName(std::string listNameStr)
{
    this->listName = std::move(listNameStr);
}

void
ListItemEntity::setListVariant(std::string listVariantStr)
{
    this->listVariant = std::move(listVariantStr);
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
                    const std::string& listName,
                    const std::string& value,
                    const PriorityEntity& priority,
                    const StatusEntity& status,
                    const time_t& dueAt,
                    const time_t& closedAt,
                    const time_t& createdAt,
                    const time_t& updatedAt)
{
    ListItemEntity listItemEntity;
    listItemEntity.setId(id);
    listItemEntity.setListName(listName);
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
                              const std::string& listName,
                              const std::string& listVariant)
{
    ListItemEntity listItemEntity;
    listItemEntity.setId(item[0]);
    listItemEntity.setValue(item[1]);
    listItemEntity.setPriority(priorityService.getPriorityFromName(item[2]));
    listItemEntity.setStatus(statusService.getStatusFromName(item[3]));
    listItemEntity.setDueAt(std::stoi(item[4]));
    listItemEntity.setClosedAt(std::stoi(item[5]));
    listItemEntity.setCreatedAt(std::stoi(item[6]));
    listItemEntity.setUpdatedAt(std::stoi(item[7]));
    listItemEntity.setListName(listName);
    listItemEntity.setListVariant(listVariant);
    return listItemEntity;
}

std::vector<std::string>
ListItemEntity::makeVector(const ListItemEntity& listItemEntity)
{
    return { *listItemEntity.getId(),
             *listItemEntity.getValue(),
             *(*listItemEntity.priority()).getName(),
             *(*listItemEntity.status()).getCommandName(),
             std::to_string(*listItemEntity.getDueAt()),
             std::to_string(*listItemEntity.getClosedAt()),
             std::to_string(*listItemEntity.getCreatedAt()),
             std::to_string(*listItemEntity.getUpdatedAt()) };
}
