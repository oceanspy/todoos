#include <stdexcept>
#include "ListEntity.h"

void ListEntity::setName(const std::string name)
{
    this->name = name;
}

void ListEntity::setType(const std::string type)
{
    for (char ch : type) {
        if (!std::isalnum(ch) && ch != '_')
        {
            throw std::invalid_argument("theme must be alphanumeric or _");
        }
    }

    this->theme = type;
}

void ListEntity::setSorting(const std::string sorting)
{
    for (char ch : sorting) {
        if (!std::isalnum(ch) && ch != '_')
        {
            throw std::invalid_argument("sorting must be alphanumeric or _");
        }
    }

    this->sorting = sorting;
}

void ListEntity::setHidden(bool hidden)
{
    this->hidden = hidden;
}

const std::string * ListEntity::getName() const
{
    if (name.empty()) {
        return nullptr;
    }

    return &name;
}

const std::string * ListEntity::getType() const
{
    if (theme.empty()) {
        return nullptr;
    }

    return &theme;
}

const std::string * ListEntity::getSorting() const
{
    if (sorting.empty()) {
        return nullptr;
    }

    return &sorting;
}

const bool * ListEntity::isHidden() const
{
    return &hidden;
}

ListEntity ListEntity::setFromVector(const std::vector <std::string>& item)
{
    bool hidden = item.at(3) == "true";

    ListEntity listEntity = ListEntity();
    listEntity.setName(item.at(0));
    listEntity.setType(item.at(1));
    listEntity.setSorting(item.at(2));
    listEntity.setHidden(hidden);

    return listEntity;
}

std::vector<std::string> ListEntity::makeVector(const ListEntity &listEntity)
{
    std::string hidden = *listEntity.isHidden() ? "true" : "false";
    std::vector <std::string> listItem = {*listEntity.getName(), *listEntity.getType(), *listEntity.getSorting(), hidden};
    return listItem;
}
