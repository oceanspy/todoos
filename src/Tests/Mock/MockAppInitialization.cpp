#include "MockAppInitialization.h"

MockAppInitialization::MockAppInitialization(IOService& ioService, const std::filesystem::path& customSubDir)
  : ioService(ioService)
  , defaultFileDataStorage("json")
  , defaultSystemFileDataStorage("json")
  , defaultConfigFileDataStorage("conf")
  , customSubDir(customSubDir)
  , homeDir(std::filesystem::temp_directory_path())
  , mainDirPath(homeDir / customSubDir)
  , appDirPath(homeDir / customSubDir)
  , configDirPath(homeDir / customSubDir)
  , cacheDirPath(homeDir / customSubDir)
  , configFileName("config")
  , cacheFileName("cache")
  , cacheFilePath(cacheDirPath / cacheFileName)
  , listOfListFileName("list")
  , configFilePath(configDirPath / configFileName)
  , defaultListFileName("default")
{
}

std::filesystem::path
MockAppInitialization::getAppDirPath()
{
    return appDirPath;
}

std::filesystem::path
MockAppInitialization::getConfigFilePath()
{
    std::filesystem::path path = appDirPath / configFileName;
    path += "." + getConfigExtension();
    return path;
}

std::filesystem::path
MockAppInitialization::getCacheFilePath()
{
    std::filesystem::path path = appDirPath / cacheFileName;
    path += "." + getConfigExtension();
    return path;
}

std::filesystem::path
MockAppInitialization::getListOfListFilePath()
{
    std::filesystem::path path = appDirPath / listOfListFileName;
    path += "." + getDefaultSystemExtension();
    return path;
}

std::filesystem::path
MockAppInitialization::getDefaultListFilePath()
{
    std::filesystem::path path = appDirPath / defaultListFileName;
    path += "." + getDefaultExtension();
    return path;
}

std::string
MockAppInitialization::getListOfListFileName()
{
    return listOfListFileName;
}

std::filesystem::path
MockAppInitialization::getHomeDir()
{
    return homeDir;
}

std::filesystem::path
MockAppInitialization::getConfigDirPath()
{
    return configDirPath;
}

std::filesystem::path
MockAppInitialization::getCacheDirPath()
{
    return cacheDirPath;
}

std::filesystem::path
MockAppInitialization::getMainDirPath()
{
    return mainDirPath;
}

const std::string
MockAppInitialization::getDefaultListName()
{
    return defaultListFileName;
}

const std::string
MockAppInitialization::getDefaultExtension()
{
    return defaultFileDataStorage;
}

const std::string
MockAppInitialization::getDefaultSystemExtension()
{
    return defaultSystemFileDataStorage;
}

const std::string
MockAppInitialization::getConfigExtension()
{
    return defaultConfigFileDataStorage;
}
