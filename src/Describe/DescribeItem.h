#pragma once

#include "../List/ListItems/ListItemEntity.h"
#include "../List/ListName.h"
#include <string>

class DescribeItem
{
  public:
    static DescribeItem create(const ListName& listName,
                               const ListItemEntity& listItem,
                               const std::string& existingDescription);
    std::string asDescriptionItemFile() const;

    const ListName& getListName() const;
    const ListItemEntity& getListItem() const;
    const std::string& getExistingDescription() const;

  private:
    DescribeItem(const ListName& listName,
                 const ListItemEntity& listItem,
                 const std::string& existingDescription);

    ListName listName;
    ListItemEntity listItem;
    std::string existingDescription;
};
