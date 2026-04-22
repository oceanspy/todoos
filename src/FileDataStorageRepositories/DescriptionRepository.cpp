#include "DescriptionRepository.h"
#include <fstream>
#include <sstream>
#include <stdexcept>

DescriptionRepository::DescriptionRepository(std::filesystem::path descriptionsDirPath)
  : descriptionsDirPath(std::move(descriptionsDirPath))
{
}

std::filesystem::path
DescriptionRepository::getFilePath(const std::string& itemId, const ListName& listName)
{
    std::string safeList = std::filesystem::path(listName.getName()).filename().string();
    std::string safeId   = std::filesystem::path(itemId).filename().string();
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

void
DescriptionRepository::remove(const std::string& itemId, const ListName& listName)
{
    std::filesystem::path filePath = getFilePath(itemId, listName);
    if (std::filesystem::exists(filePath)) {
        std::filesystem::remove(filePath);
    }
}
