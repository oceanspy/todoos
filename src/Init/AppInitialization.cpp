#include "AppInitialization.h"

#include <filesystem>

AppInitialization::AppInitialization(IOService& ioService)
  : ioService(ioService)
  , homeDir(std::filesystem::path(getenv("HOME")))
  , defaultFileDataStorage("csv")
  , defaultSystemFileDataStorage("json")
  , defaultConfigFileDataStorage("conf")
  , mainDirPath(homeDir / ".oceanspy")
  , appDirPath(homeDir / ".todoos")
  , configDirPath(homeDir / ".oceanspy/todoos")
  , cacheDirPath(homeDir / ".cache/oceanspy/todoos")
  , listOfListFileName("list")
  , configFileName("config")
  , configFilePath(configDirPath / configFileName)
  , cacheFileName("cache")
  , cacheFilePath(cacheDirPath / cacheFileName)
  , defaultListFileName("default")
{
}

std::filesystem::path
AppInitialization::getAppDirPath()
{
    return appDirPath;
}

std::filesystem::path
AppInitialization::getConfigFilePath()
{
    std::filesystem::path path = configFilePath;
    path += "." + getConfigExtension();
    return path;
}

std::filesystem::path
AppInitialization::getCacheFilePath()
{
    std::filesystem::path path = cacheFilePath;
    path += "." + getConfigExtension();
    return path;
}

std::filesystem::path
AppInitialization::getListOfListFilePath()
{
    std::filesystem::path path = appDirPath / listOfListFileName;
    path += "." + getDefaultSystemExtension();
    return path;
}

std::filesystem::path
AppInitialization::getDefaultListFilePath()
{
    std::filesystem::path path = appDirPath / defaultListFileName;
    path += "." + getDefaultExtension();
    return path;
}

std::string
AppInitialization::getListOfListFileName()
{
    return listOfListFileName;
}

std::filesystem::path
AppInitialization::getHomeDir()
{
    return homeDir;
}

std::filesystem::path
AppInitialization::getConfigDirPath()
{
    return configDirPath;
}

std::filesystem::path
AppInitialization::getCacheDirPath()
{
    return cacheDirPath;
}

std::filesystem::path
AppInitialization::getMainDirPath()
{
    return mainDirPath;
}

const std::string
AppInitialization::getDefaultListName()
{
    return defaultListFileName;
}

const std::string
AppInitialization::getDefaultExtension()
{
    return defaultFileDataStorage;
}

const std::string
AppInitialization::getDefaultSystemExtension()
{
    return defaultSystemFileDataStorage;
}

const std::string
AppInitialization::getConfigExtension()
{
    return defaultConfigFileDataStorage;
}
