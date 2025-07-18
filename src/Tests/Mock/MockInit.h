#pragma once

#include <filesystem>
#include "../../Init/InitInterface.h"
#include "../../IOService/IOService.h"

class MockInit : public InitInterface {
public:
    explicit MockInit(IOService& ioService, const std::filesystem::path& customSubDir);
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
    const std::filesystem::path& customSubDir;
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
