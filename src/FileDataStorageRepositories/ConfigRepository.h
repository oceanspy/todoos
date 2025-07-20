#ifndef CONFIGREPOSITORY_H
#define CONFIGREPOSITORY_H

#include "../Entities/ConfigEntity.h"
#include "../FileDataStorage/FileDataServiceInterface.h"
#include "../IOService/IOService.h"
#include <filesystem>
#include <fstream>
#include <string>
#include <utility>
#include <vector>

class ConfigRepository
{
  public:
    explicit ConfigRepository(FileDataServiceInterface* fileDataService, std::filesystem::path filePath);
    ConfigRepository& loadPath(const std::filesystem::path& configFilePath);
    std::vector<ConfigEntity> get();
    ConfigEntity find(const std::string& key);
    void create(ConfigEntity item);
    bool update(const std::string& key, const ConfigEntity& item);
    bool remove(std::string& key);
    void resetCache();

  private:
    std::filesystem::path filePath;
    FileDataServiceInterface* fileDataService;
    std::vector<ConfigEntity> cacheItems;
};

#endif // CONFIGREPOSITORY_H
