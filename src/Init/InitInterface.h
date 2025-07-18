#ifndef TODOOS_INITINTERFACE_H
#define TODOOS_INITINTERFACE_H

#include <filesystem>

class InitInterface {
public:
    virtual ~InitInterface() = default;

    virtual std::filesystem::path getConfigFilePath() = 0;
    virtual std::filesystem::path getCacheFilePath() = 0;
    virtual std::filesystem::path getListOfListFilePath() = 0;
    virtual std::filesystem::path getAppDirPath() = 0;
    virtual std::filesystem::path getDefaultListFilePath() = 0;
    virtual std::string getListOfListFileName() = 0;
    virtual std::filesystem::path getHomeDir() = 0;
    virtual std::filesystem::path getConfigDirPath() = 0;
    virtual std::filesystem::path getCacheDirPath() = 0;
    virtual std::filesystem::path getMainDirPath() = 0;
    virtual const std::string getDefaultListName() = 0;
    virtual const std::string getDefaultExtension() = 0;
    virtual const std::string getDefaultSystemExtension() = 0;
    virtual const std::string getConfigExtension() = 0;
};

#endif //TODOOS_INITINTERFACE_H
