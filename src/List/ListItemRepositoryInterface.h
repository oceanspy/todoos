#pragma once

#include "ListItems/ListItemEntity.h"
#include "ListName.h"
#include <string>
#include <vector>

// Storage-agnostic access to the items of a list (all variants: default, archive, delete).
class ListItemRepositoryInterface
{
  public:
    virtual ~ListItemRepositoryInterface() = default;

    virtual std::vector<ListItemEntity> get(ListName& listName) = 0;
    virtual ListItemEntity find(const std::string& id, ListName& listName) = 0;
    virtual void create(const ListItemEntity& item, ListName& listName) = 0;
    virtual bool update(const std::string& id, ListName& listName, const ListItemEntity& item) = 0;
    virtual bool remove(const std::string& id, ListName& listName) = 0;
};
