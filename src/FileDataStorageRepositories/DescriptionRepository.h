#pragma once

#include "../Describe/DescriptionRepositoryInterface.h"
#include "../List/ListName.h"
#include <filesystem>
#include <string>
#include <vector>

// File-based description repository: one markdown file per item, in descriptions/<list>/<id>.md
class DescriptionRepository : public DescriptionRepositoryInterface
{
  public:
    explicit DescriptionRepository(std::filesystem::path descriptionsDirPath);

    bool exists(const std::string& itemId, const ListName& listName) override;
    std::string load(const std::string& itemId, const ListName& listName) override;
    void save(const std::string& itemId, const ListName& listName, const std::string& content) override;
    std::vector<std::string> getIds(const ListName& listName) override;
    void remove(const std::string& itemId, const ListName& listName) override;
    std::filesystem::path getListDirPath(const ListName& listName);
    std::filesystem::path getFilePath(const std::string& itemId, const ListName& listName);

  private:
    std::filesystem::path descriptionsDirPath;
};
