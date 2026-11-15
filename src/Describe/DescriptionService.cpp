#include "DescriptionService.h"

DescriptionService::DescriptionService(std::filesystem::path cacheDirPath, DescriptionRepository& descriptionRepository)
  : cacheDirPath(std::move(cacheDirPath))
  , descriptionRepository(descriptionRepository)
{
}

std::filesystem::path
DescriptionService::getDraftFilePath(const std::string& id, const ListName& listName) const
{
    return cacheDirPath / "describe" / listName.getName() / (id + ".md");
}

void
DescriptionService::move(const std::string& id, ListName& oldListName, ListName& newListName)
{
    std::filesystem::path oldFilePath = descriptionRepository.getFilePath(id, oldListName);
    if (!std::filesystem::exists(oldFilePath)) {
        // the overwritten item in the target list may have had its own description
        descriptionRepository.remove(id, newListName);
        return;
    }

    std::filesystem::path newFilePath = descriptionRepository.getFilePath(id, newListName);
    std::filesystem::create_directories(newFilePath.parent_path());
    std::filesystem::rename(oldFilePath, newFilePath);
}

void
DescriptionService::copy(const std::string& id, ListName& oldListName, ListName& newListName)
{
    copyFile(id, oldListName, id, newListName);
}

void
DescriptionService::duplicate(const std::string& id, const std::string& newId, ListName& listName)
{
    copyFile(id, listName, newId, listName);
}

void
DescriptionService::renameList(ListName& oldListName, ListName& newListName)
{
    std::filesystem::path oldDirPath = descriptionRepository.getListDirPath(oldListName);
    if (!std::filesystem::exists(oldDirPath)) {
        return;
    }

    std::filesystem::path newDirPath = descriptionRepository.getListDirPath(newListName);
    if (!std::filesystem::exists(newDirPath)) {
        std::filesystem::create_directories(newDirPath.parent_path());
        std::filesystem::rename(oldDirPath, newDirPath);
        return;
    }

    // A leftover directory already exists for the new list: move the files one by one
    for (const auto& entry : std::filesystem::directory_iterator(oldDirPath)) {
        std::filesystem::rename(entry.path(), newDirPath / entry.path().filename());
    }
    std::filesystem::remove(oldDirPath);
}

void
DescriptionService::copyFile(const std::string& id,
                             const ListName& listName,
                             const std::string& newId,
                             const ListName& newListName)
{
    std::filesystem::path oldFilePath = descriptionRepository.getFilePath(id, listName);
    if (!std::filesystem::exists(oldFilePath)) {
        // the overwritten item in the target list may have had its own description
        descriptionRepository.remove(newId, newListName);
        return;
    }

    std::filesystem::path newFilePath = descriptionRepository.getFilePath(newId, newListName);
    std::filesystem::create_directories(newFilePath.parent_path());
    std::filesystem::copy_file(oldFilePath, newFilePath, std::filesystem::copy_options::overwrite_existing);
}
