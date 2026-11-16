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
    void move(const std::string& id, ListName& oldListName, ListName& newListName);
    void copy(const std::string& id, ListName& oldListName, ListName& newListName);
    void duplicate(const std::string& id, const std::string& newId, ListName& listName);
    void renameList(ListName& oldListName, ListName& newListName);

  private:
    void copyFile(const std::string& id,
                  const ListName& listName,
                  const std::string& newId,
                  const ListName& newListName);

    std::filesystem::path cacheDirPath;
    DescriptionRepository& descriptionRepository;
};

#endif // DESCRIPTIONSERVICE_H
