#pragma once

#include "../List/ListName.h"
#include <filesystem>
#include <string>
#include <vector>

class DescriptionRepository
{
  public:
    explicit DescriptionRepository(std::filesystem::path descriptionsDirPath);

    bool exists(const std::string& itemId, const ListName& listName);
    std::string load(const std::string& itemId, const ListName& listName);
    std::vector<std::string> getIds(const ListName& listName);
    void remove(const std::string& itemId, const ListName& listName);
    // Renames an item's description inside the same list (no-op when the item has none)
    void rename(const std::string& oldItemId, const std::string& newItemId, const ListName& listName);
    // Moves / copies an item's description to another list, under newItemId. An existing target
    // description is overwritten. No-op when the item has none.
    void move(const std::string& itemId,
              const ListName& listName,
              const std::string& newItemId,
              const ListName& newListName);
    void copy(const std::string& itemId,
              const ListName& listName,
              const std::string& newItemId,
              const ListName& newListName);
    std::filesystem::path getListDirPath(const ListName& listName);
    std::filesystem::path getFilePath(const std::string& itemId, const ListName& listName);

  private:
    std::filesystem::path descriptionsDirPath;
};
