#include "PriorityEntity.h"

const int*
PriorityEntity::getId() const
{
    return &id;
}

const int*
PriorityEntity::getPosition() const
{
    return &position;
}

const std::string*
PriorityEntity::getName() const
{
    return &name;
}

const std::string*
PriorityEntity::getIcon() const
{
    return &icon;
}

std::string
PriorityEntity::getColor() const
{
    return BashStyle::getBashCode(color);
}

void
PriorityEntity::setId(int id)
{
    this->id = id;
}

void
PriorityEntity::setPosition(int position)
{
    this->position = position;
}

void
PriorityEntity::setName(std::string name)
{
    this->name = std::move(name);
}

void
PriorityEntity::setIcon(std::string icon)
{
    this->icon = std::move(icon);
}

void
PriorityEntity::setColor(std::string color)
{
    this->color = std::move(color);
}

PriorityEntity
PriorityEntity::setFromVector(std::vector<std::string> item)
{
    PriorityEntity entity;
    // Assuming the vector has the required data in the correct order
    entity.setId(std::stoi(item[0]));
    entity.setPosition(std::stoi(item[1]));
    entity.setName(item[2]);
    entity.setIcon(item[3]);
    entity.setColor(item[4]);
    return entity;
}

std::vector<std::string>
PriorityEntity::makeVector(const PriorityEntity& priorityEntity)
{
    return std::vector<std::string>{ std::to_string(*priorityEntity.getId()),
                                     std::to_string(*priorityEntity.getPosition()),
                                     *priorityEntity.getName(),
                                     *priorityEntity.getIcon(),
                                     priorityEntity.getColor() };
}

PriorityEntity
PriorityEntity::setFromName(std::string priorityName)
{
    PriorityEntity entity;

    return entity;
}
