#include "Init.h"

#include <filesystem>

Init::Init(IOService& ioService)
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
Init::getAppDirPath()
{
    return appDirPath;
}

std::filesystem::path
Init::getConfigFilePath()
{
    std::filesystem::path path = configFilePath;
    path += "." + getConfigExtension();
    return path;
}

std::filesystem::path
Init::getCacheFilePath()
{
    std::filesystem::path path = cacheFilePath;
    path += "." + getConfigExtension();
    return path;
}

std::filesystem::path
Init::getListOfListFilePath()
{
    std::filesystem::path path = appDirPath / listOfListFileName;
    path += "." + getDefaultSystemExtension();
    return path;
}

std::filesystem::path
Init::getDefaultListFilePath()
{
    std::filesystem::path path = appDirPath / defaultListFileName;
    path += "." + getDefaultExtension();
    return path;
}

std::string
Init::getListOfListFileName()
{
    return listOfListFileName;
}

std::filesystem::path
Init::getHomeDir()
{
    return homeDir;
}

std::filesystem::path
Init::getConfigDirPath()
{
    return configDirPath;
}

std::filesystem::path
Init::getCacheDirPath()
{
    return cacheDirPath;
}

std::filesystem::path
Init::getMainDirPath()
{
    return mainDirPath;
}

const std::string
Init::getDefaultListName()
{
    return defaultListFileName;
}

const std::string
Init::getDefaultExtension()
{
    return defaultFileDataStorage;
}

const std::string
Init::getDefaultSystemExtension()
{
    return defaultSystemFileDataStorage;
}

const std::string
Init::getConfigExtension()
{
    return defaultConfigFileDataStorage;
}
