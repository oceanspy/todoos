#pragma once

#include "ListEntity.h"
#include <string>
#include <vector>

// Storage-agnostic access to lists. A list owns the storage of its items:
// creating, renaming or removing a list also creates, renames or removes
// whatever holds its items (files, rows...).
class ListRepositoryInterface
{
  public:
    virtual ~ListRepositoryInterface() = default;

    virtual std::vector<ListEntity> get() = 0;
    virtual ListEntity find(const std::string& name) = 0;
    // Registers the list and creates the storage for its items.
    virtual void create(const ListEntity& item) = 0;
    // Updates list metadata only. Use rename() to change the name.
    virtual bool update(const std::string& name, const ListEntity& item) = 0;
    // Renames the list and moves the storage of its items.
    virtual void rename(const std::string& oldName, const std::string& newName) = 0;
    // Unregisters the list and permanently deletes its items.
    virtual void remove(const std::string& name) = 0;
    // Unregisters the list but keeps its items as a recoverable backup.
    virtual void removeKeepingBackup(const std::string& name) = 0;
};
