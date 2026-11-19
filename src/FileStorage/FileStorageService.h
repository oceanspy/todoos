#ifndef TODOOS_FILESTORAGESERVICE_H
#define TODOOS_FILESTORAGESERVICE_H

#include "../Config/ConfigService.h"
#include <filesystem>
#include <fstream>
#include <string>

// File-storage helper: manages the three item files (default, archive, delete)
// backing a list. Only used by the file-based ListRepository.
class FileStorageService
{
  public:
    explicit FileStorageService(ConfigService& configService);
    static const std::string CSV;
    static const std::string JSON;
    void moveFileTo(const std::string& oldListName, const std::string& newListName);
    void createNewListFile(const std::string& listName);
    void removeListFile(const std::string& listName);

  private:
    ConfigService& configService;
    void populateNewListFile(std::ofstream& outfile);
};

#endif // TODOOS_FILESTORAGESERVICE_H
