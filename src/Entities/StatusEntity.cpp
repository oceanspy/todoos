#include "StatusEntity.h"

const int * StatusEntity::getId() const {
    return &id;
}

const int * StatusEntity::getPosition() const {
    return &position;
}

const std::string * StatusEntity::getName() const {
    return &name;
}

const std::string * StatusEntity::getCommandName() const {
    return &command;
}

const std::string * StatusEntity::getIcon() const {
    return &icon;
}

const int StatusEntity::getIconLength() const
{
    return iconLength;
}

std::string StatusEntity::getColor() const {
    return BashStyle::getBashCode(color);
}

std::string StatusEntity::getStyle() const {
    return BashStyle::getBashCode(style);
}

const bool * StatusEntity::isClosed() const {
    return &statusIsClosed;
}

const bool * StatusEntity::isCancelled() const {
    return &statusIsCancelled;
}

const bool * StatusEntity::isPassive() const
{
    return &statusIsPassive;
}

void StatusEntity::setId(int id) {
    this->id = id;
}

void StatusEntity::setPosition(int position) {
    this->position = position;
}

void StatusEntity::setName(std::string name) {
    this->name = std::move(name);
}

void StatusEntity::setCommandName(std::string command) {
    this->command = std::move(command);
}

void StatusEntity::setIcon(std::string icon) {
    this->icon = std::move(icon);
}

void StatusEntity::setIconLength(int iconLength)
{
    this->iconLength = iconLength;
}

void StatusEntity::setColor(std::string color) {
    this->color = std::move(color);
}

void StatusEntity::setStyle(std::string style) {
    this->style = std::move(style);
}

void StatusEntity::setClosed(bool closed) {
    this->statusIsClosed = closed;
}

void StatusEntity::setCancelled(bool cancelled) {
    this->statusIsCancelled = cancelled;
}

void StatusEntity::setPassive(bool passive) {
    this->statusIsPassive = passive;
}

StatusEntity StatusEntity::setFromVector(std::vector<std::string> item) {
    StatusEntity entity;
    // Assuming the vector has the required data in the correct order
    entity.setId(std::stoi(item[0]));
    entity.setPosition(std::stoi(item[1]));
    entity.setName(item[2]);
    entity.setCommandName(item[3]);
    entity.setIcon(item[4]);
    entity.setIconLength(std::stoi(item[5]));
    entity.setColor(item[6]);
    entity.setStyle(item[7]);
    entity.setClosed(item[8] == "true");
    entity.setCancelled(item[9] == "true");
    entity.setPassive(item[10] == "true");
    return entity;
}

std::vector<std::string> StatusEntity::makeVector(const StatusEntity& statusEntity) {
    return std::vector<std::string> {
        std::to_string(*statusEntity.getId()),
        std::to_string(*statusEntity.getPosition()),
        *statusEntity.getName(),
        *statusEntity.getCommandName(),
        *statusEntity.getIcon(),
        std::to_string(statusEntity.getIconLength()),
        statusEntity.getColor(),
        statusEntity.getStyle(),
        *statusEntity.isClosed() ? "true" : "false",
        *statusEntity.isCancelled() ? "true" : "false",
        *statusEntity.isPassive() ? "true" : "false"
    };
}
