#pragma once

#include "../List/ListName.h"
#include <filesystem>
#include <string>

class DescriptionRepository
{
  public:
    explicit DescriptionRepository(std::filesystem::path descriptionsDirPath);

    bool exists(const std::string& itemId, const ListName& listName);
    std::string load(const std::string& itemId, const ListName& listName);
    void remove(const std::string& itemId, const ListName& listName);
    std::filesystem::path getFilePath(const std::string& itemId, const ListName& listName);

  private:
    std::filesystem::path descriptionsDirPath;
};
