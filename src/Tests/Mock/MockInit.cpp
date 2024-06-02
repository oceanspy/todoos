#include "MockInit.h"

MockInit::MockInit(IOService& ioService, const std::filesystem::path& customSubDir)
        : ioService(ioService),
          defaultFileDataStorage("json"),
          defaultSystemFileDataStorage("json"),
          defaultConfigFileDataStorage("conf"),
          customSubDir(customSubDir),
          homeDir(std::filesystem::temp_directory_path()),
          mainDirPath(homeDir / customSubDir),
          appDirPath(homeDir / customSubDir),
          configDirPath(homeDir / customSubDir),
          configFileName("config"),
          listOfListFileName("list"),
          configFilePath(configDirPath / configFileName),
          defaultListFileName("default")
{

}

std::filesystem::path MockInit::getAppDirPath()
{
    return appDirPath;
}

std::filesystem::path MockInit::getConfigFilePath()
{
    std::filesystem::path path = appDirPath / configFileName;
    path += "." + getConfigExtension();
    return path;
}

std::filesystem::path MockInit::getListOfListFilePath()
{
    std::filesystem::path path = appDirPath / listOfListFileName;
    path += "." + getDefaultSystemExtension();
    return path;
}

std::filesystem::path MockInit::getDefaultListFilePath()
{
    std::filesystem::path path = appDirPath / defaultListFileName;
    path += "." + getDefaultExtension();
    return path;
}

std::string MockInit::getListOfListFileName() {
    return listOfListFileName;
}

std::filesystem::path MockInit::getHomeDir() {
    return homeDir;
}

std::filesystem::path MockInit::getConfigDirPath() {
    return configDirPath;
}

std::filesystem::path MockInit::getMainDirPath() {
    return mainDirPath;
}

const std::string MockInit::getDefaultListName() {
    return defaultListFileName;
}

const std::string MockInit::getDefaultExtension() {
    return defaultFileDataStorage;
}

const std::string MockInit::getDefaultSystemExtension() {
    return defaultSystemFileDataStorage;
}

const std::string MockInit::getConfigExtension() {
    return defaultConfigFileDataStorage;
}
