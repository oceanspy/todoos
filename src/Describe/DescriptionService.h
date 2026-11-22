#ifndef DESCRIPTIONSERVICE_H
#define DESCRIPTIONSERVICE_H

#include "../FileDataStorageRepositories/DescriptionRepository.h"
#include "../List/ListName.h"
#include <filesystem>
#include <stdexcept>
#include <string>

class DescriptionService
{
  public:
    DescriptionService(std::filesystem::path cacheDirPath, DescriptionRepository& descriptionRepository);
    std::filesystem::path getDraftFilePath(const std::string& id, const ListName& listName) const;
    void renameList(ListName& oldListName, ListName& newListName);

  private:
    std::filesystem::path cacheDirPath;
    DescriptionRepository& descriptionRepository;
};

#endif // DESCRIPTIONSERVICE_H
