#ifndef TODOOS_APPINSTALLATION_H
#define TODOOS_APPINSTALLATION_H

#include "../Serializers/DataSerializerInterface.h"
#include "../IOService/IOService.h"
#include "AppInitializationInterface.h"
#include "AppInstallationInterface.h"
#include <filesystem>
#include <fstream>

class AppInstallation : public AppInstallationInterface
{
  public:
    AppInstallation(IOService& ioService,
                 DataSerializerInterface& storageJsonService,
                 DataSerializerInterface& storageCsvService,
                 DataSerializerInterface& configStorageService,
                 AppInitializationInterface& init);
    bool isNew() override;
    void make() override;
    void populate() override;
    bool wipe() override;

  private:
    IOService& ioService;
    DataSerializerInterface& storageJsonService;
    DataSerializerInterface& storageCsvService;
    DataSerializerInterface& configStorageService;
    AppInitializationInterface& init;
    void createDirectories();
    void createConfigFile();
    void createCacheFile();
    void createListOfListFile();
    void createListFile(const std::string& listName);
    void populateNewListFile(std::ofstream& outfile);
};

#endif // TODOOS_APPINSTALLATION_H
