#include "DescriptionRepository.h"
#include "../List/ListItemId.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>

DescriptionRepository::DescriptionRepository(std::filesystem::path descriptionsDirPath)
  : descriptionsDirPath(std::move(descriptionsDirPath))
{
}

std::filesystem::path
DescriptionRepository::getListDirPath(const ListName& listName)
{
    std::string safeList = std::filesystem::path(listName.getName()).filename().string();
    return descriptionsDirPath / safeList;
}

std::filesystem::path
DescriptionRepository::getFilePath(const std::string& itemId, const ListName& listName)
{
    std::string safeList = std::filesystem::path(listName.getName()).filename().string();
    std::string safeId = std::filesystem::path(itemId).filename().string();
    return descriptionsDirPath / safeList / (safeId + ".md");
}

bool
DescriptionRepository::exists(const std::string& itemId, const ListName& listName)
{
    return std::filesystem::exists(getFilePath(itemId, listName));
}

std::string
DescriptionRepository::load(const std::string& itemId, const ListName& listName)
{
    std::filesystem::path filePath = getFilePath(itemId, listName);

    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open description file: " + filePath.string());
    }

    std::ostringstream content;
    content << file.rdbuf();
    return content.str();
}

std::vector<std::string>
DescriptionRepository::getIds(const ListName& listName)
{
    auto dirPath = getListDirPath(listName);

    std::vector<std::string> availableIdsWithDescription = {};
    if (!std::filesystem::exists(dirPath)) {
        return availableIdsWithDescription;
    }
    for (const auto& entry :
         std::filesystem::directory_iterator(dirPath, std::filesystem::directory_options::skip_permission_denied)) {
        std::string id = entry.path().filename().string();
        id.resize(ListItemId::getIdLength()); // keep only the id
        availableIdsWithDescription.push_back(id);
    }

    return availableIdsWithDescription;
}

void
DescriptionRepository::remove(const std::string& itemId, const ListName& listName)
{
    std::filesystem::path filePath = getFilePath(itemId, listName);
    if (std::filesystem::exists(filePath)) {
        std::filesystem::remove(filePath);
    }
}
