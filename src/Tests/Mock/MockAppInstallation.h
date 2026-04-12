#ifndef TODOOS_MOCKINSTALLATION_H
#define TODOOS_MOCKINSTALLATION_H

#include "../../IOService/IOService.h"
#include "../../Init/AppInitializationInterface.h"
#include "../../Init/AppInstallationInterface.h"
#include "../../Serializers/DataSerializerInterface.h"
#include <fstream>

class MockAppInstallation : public AppInstallationInterface
{
  public:
    MockAppInstallation(IOService& ioService,
                        DataSerializerInterface& storageService,
                        DataSerializerInterface& configStorageService,
                        AppInitializationInterface& init);
    bool isNew() override;
    void make() override;
    void populate() override;
    bool wipe() override;
    void createListFile(const std::string& listName);

  private:
    IOService& ioService;
    DataSerializerInterface& storageService;
    DataSerializerInterface& configStorageService;
    AppInitializationInterface& init;
    void createDirectories();
    void createConfigFile();
    void createCacheFile();
    void createListOfListFile();
    void populateNewListFile(std::ofstream& outfile);
};

#endif // TODOOS_MOCKINSTALLATION_H
