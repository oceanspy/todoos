#ifndef TODOOS_MOCKINSTALLATION_H
#define TODOOS_MOCKINSTALLATION_H

#include "../../FileDataStorage/FileDataServiceInterface.h"
#include "../../IOService/IOService.h"
#include "../../Init/InitInterface.h"
#include "../../Init/InstallationInterface.h"
#include <fstream>

class MockInstallation : public InstallationInterface
{
  public:
    MockInstallation(IOService& ioService,
                     FileDataServiceInterface& storageService,
                     FileDataServiceInterface& configStorageService,
                     InitInterface& init);
    bool isNew() override;
    void make() override;
    void populate() override;
    bool wipe() override;

  private:
    IOService& ioService;
    FileDataServiceInterface& storageService;
    FileDataServiceInterface& configStorageService;
    InitInterface& init;
    void createDirectories();
    void createConfigFile();
    void createCacheFile();
    void createListOfListFile();
    void createListFile(const std::string& listName);
    void populateNewListFile(std::ofstream& outfile);
};

#endif // TODOOS_MOCKINSTALLATION_H
