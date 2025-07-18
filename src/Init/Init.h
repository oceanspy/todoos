#ifndef INIT_H
#define INIT_H

#include "InitInterface.h"
#include "../IOService/IOService.h"
#include <filesystem>
#include "../Entities/ListEntity.h"
#include "../FileDataStorage/JSONService.h"

class Init: public InitInterface {
public:
    explicit Init(IOService& ioService);
    std::filesystem::path getConfigFilePath() override;
    std::filesystem::path getCacheFilePath() override;
    std::filesystem::path getHomeDir() override;
    std::filesystem::path getConfigDirPath() override;
    std::filesystem::path getCacheDirPath() override;
    std::filesystem::path getMainDirPath() override;
    std::filesystem::path getListOfListFilePath() override;
    std::string getListOfListFileName() override;
    std::filesystem::path getAppDirPath() override;
    std::filesystem::path getDefaultListFilePath() override;
    const std::string getDefaultListName() override;
    const std::string getDefaultExtension() override;
    const std::string getDefaultSystemExtension() override;
    const std::string getConfigExtension() override;
private:
    IOService& ioService;
    std::string defaultFileDataStorage;
    std::string defaultSystemFileDataStorage;
    std::string defaultConfigFileDataStorage;
    std::filesystem::path homeDir;
    std::filesystem::path mainDirPath;
    std::filesystem::path appDirPath;
    std::filesystem::path configDirPath;
    std::filesystem::path configFileName;
    std::filesystem::path configFilePath;
    std::filesystem::path cacheDirPath;
    std::filesystem::path cacheFileName;
    std::filesystem::path cacheFilePath;
    std::filesystem::path listOfListFileName;
    std::filesystem::path defaultListFileName;
};



#endif //INIT_H
