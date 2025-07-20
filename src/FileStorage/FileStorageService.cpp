#include "FileStorageService.h"

FileStorageService::FileStorageService(IOService& ioService, ConfigService& configService)
  : ioService(ioService)
  , configService(configService)
{
}

void
FileStorageService::createNewListFile(const std::string& listName)
{
    std::filesystem::path filePaths[3];
    filePaths[0] = configService.getListFilePath(listName);
    filePaths[1] = configService.getListArchiveFilePath(listName);
    filePaths[2] = configService.getListDeleteFilePath(listName);
    for (const auto& filePath : filePaths) {
        std::ofstream outfile(filePath, std::ios::out | std::ios::app);
        if (!outfile.is_open()) {
            ioService.br();
            ioService.error("Failed to create application load file");
            ioService.br();
            return;
        }
        populateNewListFile(outfile);
        outfile.close();
    }
    return;
}

void
FileStorageService::removeListFile(const std::string& listName)
{
    std::filesystem::path listPath = configService.getListFilePath(listName);
    std::filesystem::path listArchivePath = configService.getListArchiveFilePath(listName);
    std::filesystem::path listDeletePath = configService.getListDeleteFilePath(listName);

    if (!std::filesystem::exists(listPath)) {
        throw std::invalid_argument("File does not exist: " + listPath.string());
    }

    try {
        std::filesystem::remove(listPath);
        std::filesystem::remove(listArchivePath);
        std::filesystem::remove(listDeletePath);
    } catch (std::filesystem::filesystem_error& e) {
        throw std::invalid_argument("Failed to remove file: " + listPath.string());
    }
}

void
FileStorageService::moveFileTo(const std::string& oldListName, const std::string& newListName)
{
    std::filesystem::path oldListPath = configService.getListFilePath(oldListName);
    std::filesystem::path oldListArchivePath = configService.getListArchiveFilePath(oldListName);
    std::filesystem::path oldListDeletePath = configService.getListDeleteFilePath(oldListName);
    std::filesystem::path newListPath = configService.getListFilePath(newListName);
    std::filesystem::path newListArchivePath = configService.getListArchiveFilePath(newListName);
    std::filesystem::path newListDeletePath = configService.getListDeleteFilePath(newListName);

    if (std::filesystem::exists(newListPath)) {
        throw std::invalid_argument("File already exists: " + newListName);
    }

    // move file
    std::filesystem::rename(oldListPath, newListPath);
    std::filesystem::rename(oldListArchivePath, newListArchivePath);
    std::filesystem::rename(oldListDeletePath, newListDeletePath);
}

void
FileStorageService::populateNewListFile(std::ofstream& outfile)
{
    if (configService.getFileDataStorageType() == "json") {
        outfile << "[\n";
        outfile << "]";
    } else if (configService.getFileDataStorageType() == "csv") {
        outfile << "# ID,VALUE,PRIORITY,STATUS,DUE_AT,CLOSED_AT,CREATED_AT,UPDATED_AT\n";
    }
}
