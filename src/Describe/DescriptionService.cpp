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
