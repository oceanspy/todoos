#ifndef TODOOS_FILESTORAGESERVICE_H
#define TODOOS_FILESTORAGESERVICE_H

#include "../Config/ConfigService.h"
#include "../Entities/ListEntity.h"
#include "../FileDataStorage/JSONService.h"
#include "../IOService/IOService.h"
#include <filesystem>
#include <fstream>

class FileStorageService
{
  public:
    FileStorageService(IOService& ioService, ConfigService& configService);
    void moveFileTo(const std::string& oldListName, const std::string& newListName);
    void createNewListFile(const std::string& listName);
    void removeListFile(const std::string& listName);

  private:
    IOService& ioService;
    ConfigService& configService;
    void populateNewListFile(std::ofstream& outfile);
};

#endif // TODOOS_FILESTORAGESERVICE_H
