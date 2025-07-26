#ifndef CONFIGSERVICE_H
#define CONFIGSERVICE_H

#include "../Command/Command.h"
#include "../FileDataStorageRepositories/ConfigRepository.h"
#include "../IOService/IOService.h"
#include "../Init/InitInterface.h"
#include <string>
#include <vector>

class ConfigService
{
  public:
    ConfigService(IOService& ioService,
                  InitInterface& init,
                  ConfigRepository& configRepository,
                  ConfigRepository& cacheRepository,
                  Command& command);
    std::vector<ConfigEntity> get();
    std::string getValue(const std::string& key);
    bool isTrue(const std::string& key);
    ConfigEntity find(const std::string& key);
    void add(std::string key, std::string value);
    void edit(const std::string& key, std::string value);
    void editCurrentList(std::string value);
    void remove(std::string& key);
    std::filesystem::path getAppDirPath();
    std::string getFileDataStorageType();
    std::string getDefaultSystemExtension();
    std::string getUsedListNameStr();
    std::string getUsedListVariantStr();
    std::string getDefaultList();
    std::filesystem::path getListofListFilePath();
    std::filesystem::path getListFilePath(const std::string& listName);
    std::filesystem::path getListArchiveFilePath(const std::string& listName);
    std::filesystem::path getListDeleteFilePath(const std::string& listName);
    std::filesystem::path getCurrentListFilePath();
    std::filesystem::path getListArchiveFilePathFromFilePath(std::filesystem::path listFilePath);
    std::filesystem::path getListDeleteFilePathFromFilePath(std::filesystem::path listFilePath);

  private:
    IOService& ioService;
    InitInterface& init;
    ConfigRepository& configRepository;
    ConfigRepository& cacheRepository;
    Command& command;
    ConfigRepository getRepository(const std::string& key);
};

#endif // CONFIGSERVICE_H
