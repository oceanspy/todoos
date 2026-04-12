#include "AppInstallation.h"

AppInstallation::AppInstallation(IOService& ioService,
                           DataSerializerInterface& storageJsonService,
                           DataSerializerInterface& storageCsvService,
                           DataSerializerInterface& configStorageService,
                           AppInitializationInterface& init)
  : ioService(ioService)
  , storageJsonService(storageJsonService)
  , storageCsvService(storageCsvService)
  , configStorageService(configStorageService)
  , init(init)
{
}

bool
AppInstallation::isNew()
{
    return !std::filesystem::exists(init.getConfigFilePath());
}

void
AppInstallation::make()
{
    ioService.info("First launch detected.");
    ioService.message("Inializing program...");
    this->createDirectories();
    this->createConfigFile();
    this->createCacheFile();
    this->createListOfListFile();
    this->createListFile(init.getDefaultListName());
    this->populate();
    ioService.success("Program initialized.");
    ioService.message("You can adapt your settings in " + init.getConfigFilePath().string());
    ioService.info("Welcome! :-)");
}

void
AppInstallation::populate()
{
    std::vector<std::vector<std::string>> defaultConfigData = { { "appDirStorage", init.getAppDirPath().string() },
                                                                { "fileDataStorageType", "csv" },
                                                                { "defaultList", "default" },
                                                                { "theme", "default" },
                                                                { "consoleRowMaxLength", "0" },
                                                                { "archiveWhenCompleted", "false" },
                                                                { "idRandomGenerationType", "letters" } };
    configStorageService.load(init.getConfigFilePath());
    configStorageService.write(defaultConfigData);

    std::vector<std::vector<std::string>> defaultCacheData = {
        { "currentList", "default" },
    };
    configStorageService.load(init.getCacheFilePath());
    configStorageService.write(defaultCacheData);

    std::vector<std::vector<std::string>> defaultListOfListData = { { "default", "default", "default", "false" } };
    storageJsonService.load(init.getListOfListFilePath());
    storageJsonService.write(defaultListOfListData);

    std::vector<std::vector<std::string>> defaultListData = {
        { "aaaa",
          "Welcome to todoos!",
          "urgent",
          "to-do",
          "0",
          "0",
          std::to_string(time(nullptr)),
          std::to_string(time(nullptr)) },
        { "bbbb",
          "Please look at 'todoos -h' to get started!",
          "high",
          "to-do",
          "0",
          "0",
          std::to_string(time(nullptr)),
          std::to_string(time(nullptr)) },
        { "cccc",
          "Add your first task by doing: 'todoos add \"My first task\"'",
          "medium",
          "to-do",
          "0",
          "0",
          std::to_string(time(nullptr)),
          std::to_string(time(nullptr)) },
        { "dddd", "Enjoy! :-)", "low", "to-do", "0", "0", std::to_string(time(nullptr)), std::to_string(time(nullptr)) }
    };
    storageCsvService.load(init.getDefaultListFilePath());
    storageCsvService.write(defaultListData);
}

void
AppInstallation::createDirectories()
{
    std::error_code ec;

    std::filesystem::create_directories(init.getMainDirPath(), ec);
    if (ec) {
        ioService.error("Failed to create config main directory");
        return;
    }

    std::filesystem::create_directories(init.getCacheDirPath(), ec);
    if (ec) {
        ioService.error("Failed to create cache directory");
        return;
    }

    std::filesystem::create_directories(init.getConfigDirPath(), ec);
    if (ec) {
        ioService.error("Failed to create config directory");
        return;
    }

    std::filesystem::create_directories(init.getAppDirPath(), ec);
    if (ec) {
        ioService.error("Failed to create application directory");
        return;
    }
}

void
AppInstallation::createConfigFile()
{
    std::ofstream outfile(init.getConfigFilePath(), std::ios::out | std::ios::app);
    if (!outfile.is_open()) {
        ioService.error("Failed to create application config file");
        return;
    }
    outfile.close();
}

void
AppInstallation::createCacheFile()
{
    std::ofstream outfile(init.getCacheFilePath(), std::ios::out | std::ios::app);
    if (!outfile.is_open()) {
        ioService.error("Failed to create cache file");
        return;
    }
    outfile.close();
}

void
AppInstallation::createListOfListFile()
{
    std::ofstream outfile(init.getListOfListFilePath(), std::ios::out | std::ios::app);
    if (!outfile.is_open()) {
        ioService.error("Failed to create application list of list file");
        return;
    }
    outfile.close();
}

void
AppInstallation::createListFile(const std::string& listName)
{
    std::filesystem::path filePaths[3];
    filePaths[0] = init.getAppDirPath() / listName;
    filePaths[0] += "." + init.getDefaultExtension();
    filePaths[1] = init.getAppDirPath() / ".archive_";
    filePaths[1] += listName;
    filePaths[1] += "." + init.getDefaultExtension();
    filePaths[2] = init.getAppDirPath() / ".del_";
    filePaths[2] += listName;
    filePaths[2] += "." + init.getDefaultExtension();
    for (const auto& filePath : filePaths) {
        std::ofstream outfile(filePath, std::ios::out | std::ios::app);
        if (!outfile.is_open()) {
            ioService.print("Failed to create default application file");
            return;
        }
        populateNewListFile(outfile);
        outfile.close();
    }
}

bool
AppInstallation::wipe()
{
    // TODO: Implement wipe method
    return false;
}

void
AppInstallation::populateNewListFile(std::ofstream& outfile)
{
    if (init.getDefaultExtension() == "json") {
        outfile << "[\n";
        outfile << "]";
    } else if (init.getDefaultExtension() == "csv") {
        outfile << "# ID,VALUE,PRIORITY,STATUS,DUE_AT,CLOSED_AT,CREATED_AT,UPDATED_AT\n";
    }
}
