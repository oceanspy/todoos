#ifndef TODOOS_FILESTORAGESERVICE_H
#define TODOOS_FILESTORAGESERVICE_H

#include "../IOService/IOService.h"
#include <filesystem>
#include <fstream>
#include "../Config/ConfigService.h"
#include "../FileDataStorage/JSONService.h"
#include "../Entities/ListEntity.h"

class FileStorageService {
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


#endif //TODOOS_FILESTORAGESERVICE_H
