#include "ConfigService.h"

#include <stdexcept>
#include <utility>

ConfigService::ConfigService(IOService& ioService,
                             InitInterface& init,
                             ConfigRepository& configRepository,
                             ConfigRepository& cacheRepository,
                             Command& command)
  : ioService(ioService)
  , init(init)
  , configRepository(configRepository)
  , cacheRepository(cacheRepository)
  , command(command)
{
}

std::vector<ConfigEntity>
ConfigService::get()
{
    return configRepository.get();
}

std::string
ConfigService::getValue(const std::string& key)
{
    std::string value;
    try {
        value = *find(key).getValue();
    } catch (const std::invalid_argument& e) {
        value = "";
    }
    return value;
}

bool
ConfigService::isTrue(const std::string& key)
{
    std::string value = getValue(key);
    return value == "true";
}

ConfigEntity
ConfigService::find(const std::string& key)
{
    ConfigRepository repository = getRepository(key);
    std::vector<ConfigEntity> configs = repository.get();
    for (ConfigEntity configEntity : configs) {
        if (*configEntity.getKey() == key) {
            return configEntity;
        }
    }
    throw std::invalid_argument("No config were found with the key: " + key);
}

void
ConfigService::add(std::string key, std::string value)
{
    ConfigEntity configEntity = ConfigEntity();
    configEntity.setKey(std::move(key));
    configEntity.setValue(std::move(value));
    ConfigRepository repository = getRepository(key);
    repository.create(configEntity);
}

void
ConfigService::edit(const std::string& key, std::string value)
{
    ConfigEntity configEntity = find(key);
    configEntity.setValue(std::move(value));
    ConfigRepository repository = getRepository(key);
    repository.update(key, configEntity);
}

void
ConfigService::editCurrentList(std::string value)
{
    std::string key = "currentList";
    ConfigEntity cacheEntity = find(key);
    cacheEntity.setValue(std::move(value));
    ConfigRepository repository = getRepository(key);
    repository.update(key, cacheEntity);
}

void
ConfigService::remove(std::string& key)
{
    ConfigRepository repository = getRepository(key);
    repository.remove(key);
}

ConfigRepository
ConfigService::getRepository(const std::string& key)
{
    if (key == "currentList") {
        return cacheRepository;
    } else if (key == "appDirStorage") {
        return configRepository;
    } else if (key == "fileDataStorageType") {
        return configRepository;
    } else if (key == "defaultList") {
        return configRepository;
    } else if (key == "theme") {
        return configRepository;
    } else if (key == "consoleRowMaxLength") {
        return configRepository;
    } else if (key == "archiveWhenCompleted") {
        return configRepository;
    } else if (key == "idRandomGenerationType") {
        return configRepository;
    }

    throw std::invalid_argument("No config were found with the key: " + key);
}

std::filesystem::path
ConfigService::getAppDirPath()
{
    ConfigEntity configEntity = configRepository.find("appDirStorage");
    return *configEntity.getValue();
}

std::string
ConfigService::getFileDataStorageType()
{
    ConfigEntity configEntity = configRepository.find("fileDataStorageType");
    return *configEntity.getValue();
}

std::string
ConfigService::getDefaultSystemExtension()
{
    return init.getDefaultSystemExtension();
}

std::string
ConfigService::getDefaultList()
{
    ConfigEntity configEntity = configRepository.find("defaultList");
    return *configEntity.getValue();
}

std::string
ConfigService::getCurrentList()
{
    if (!command.getOptions().empty() && command.hasOption("list")) {
        return command.getOption("list");
    }

    ConfigEntity cacheEntity = cacheRepository.find("currentList");
    return *cacheEntity.getValue();
}

std::string
ConfigService::getCurrentListVariant()
{
    if (!command.getOptions().empty() && command.hasOption("archive")) {
        return "archive";
    } else if (!command.getOptions().empty() && command.hasOption("delete")) {
        return "delete";
    }

    return "default";
}

std::filesystem::path
ConfigService::getListofListFilePath()
{
    std::filesystem::path path = getAppDirPath() / init.getListOfListFileName();
    path += "." + init.getDefaultSystemExtension();
    return path;
}

std::filesystem::path
ConfigService::getListFilePath(const std::string& listName)
{
    std::filesystem::path fileName = listName + "." + getFileDataStorageType();
    return getAppDirPath() / fileName;
}

std::filesystem::path
ConfigService::getListArchiveFilePath(const std::string& listName)
{
    std::filesystem::path fileName = ".archive_" + listName + "." + getFileDataStorageType();
    return getAppDirPath() / fileName;
}

std::filesystem::path
ConfigService::getListDeleteFilePath(const std::string& listName)
{
    std::filesystem::path fileName = ".del_" + listName + "." + getFileDataStorageType();
    return getAppDirPath() / fileName;
}

std::filesystem::path
ConfigService::getCurrentListFilePath()
{
    return getListFilePath(getCurrentList());
}

std::filesystem::path
ConfigService::getListArchiveFilePathFromFilePath(std::filesystem::path listFilePath)
{
    if (listFilePath.filename().string().find(".archive_") != std::string::npos) {
        return listFilePath;
    }
    return listFilePath.replace_filename(".archive_" + listFilePath.filename().string());
}

std::filesystem::path
ConfigService::getListDeleteFilePathFromFilePath(std::filesystem::path listFilePath)
{
    if (listFilePath.filename().string().find(".del_") != std::string::npos) {
        return listFilePath;
    }
    return listFilePath.replace_filename(".del_" + listFilePath.filename().string());
}
