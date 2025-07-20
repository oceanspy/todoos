#ifndef TODOOS_INSTALLATION_H
#define TODOOS_INSTALLATION_H

#include "../FileDataStorage/FileDataServiceInterface.h"
#include "../IOService/IOService.h"
#include "InitInterface.h"
#include "InstallationInterface.h"
#include <filesystem>
#include <fstream>

class Installation : public InstallationInterface
{
  public:
    Installation(IOService& ioService,
                 FileDataServiceInterface& storageJsonService,
                 FileDataServiceInterface& storageCsvService,
                 FileDataServiceInterface& configStorageService,
                 InitInterface& init);
    bool isNew() override;
    void make() override;
    void populate() override;
    bool wipe() override;

  private:
    IOService& ioService;
    FileDataServiceInterface& storageJsonService;
    FileDataServiceInterface& storageCsvService;
    FileDataServiceInterface& configStorageService;
    InitInterface& init;
    void createDirectories();
    void createConfigFile();
    void createCacheFile();
    void createListOfListFile();
    void createListFile(const std::string& listName);
    void populateNewListFile(std::ofstream& outfile);
};

#endif // TODOOS_INSTALLATION_H
