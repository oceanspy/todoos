#pragma once

#include "../List/ListName.h"
#include <string>
#include <vector>

// Storage-agnostic access to item descriptions.
class DescriptionRepositoryInterface
{
  public:
    virtual ~DescriptionRepositoryInterface() = default;

    virtual bool exists(const std::string& itemId, const ListName& listName) = 0;
    virtual std::string load(const std::string& itemId, const ListName& listName) = 0;
    virtual void save(const std::string& itemId, const ListName& listName, const std::string& content) = 0;
    virtual std::vector<std::string> getIds(const ListName& listName) = 0;
    virtual void remove(const std::string& itemId, const ListName& listName) = 0;
};
