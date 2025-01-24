#include "MockInstallation.h"

MockInstallation::MockInstallation(IOService& ioService, FileDataServiceInterface& storageService, FileDataServiceInterface& configStorageService, InitInterface& init)
        : ioService(ioService),
          storageService(storageService),
          configStorageService(configStorageService),
          init(init)
{

}

bool MockInstallation::isNew()
{
    return !std::filesystem::exists(init.getConfigFilePath());
}

void MockInstallation::make()
{
    ioService.info("First launch detected.");
    ioService.print("MockInstallationializing program...");
    this->createDirectories();
    this->createConfigFile();
    this->createListOfListFile();
    this->createListFile(init.getDefaultListName());
    this->createListFile("tempListName");
    this->createListFile("tempList2Name");
    this->populate();
    ioService.success("Program initialized.");
    ioService.print("You can adapt your settings in " + init.getConfigFilePath().string());
    ioService.info("Welcome! :-)");
}

void MockInstallation::populate()
{
    std::vector <std::vector <std::string>> defaultConfigData = {
            { "appDirStorage", init.getAppDirPath().string() },
            { "fileDataStorageType", "json" },
            { "currentList", "tempListName" },
            { "defaultList", "default" },
            { "theme", "default" },
            { "consoleRowMaxLength", "96" },
            { "archiveWhenCompleted", "false" },
            { "idRandomGenerationType", "letters" }

    };

    configStorageService.load(init.getConfigFilePath());
    configStorageService.write(defaultConfigData);

    std::vector <std::vector <std::string>> defaultListofListData = {
            { "tempListName", "default", "default", "false" },
            { "tempList2Name", "default", "default", "false" }
    };

    storageService.load(init.getListOfListFilePath());
    storageService.write(defaultListofListData);

    std::vector <std::vector <std::string>> defaultListData = {
            {"aaaa", "test 1", "high", "to-do", "0", "0", "1712487259", "1712487259"},
            {"bbbb", "test 2", "medium", "started", "1712487259", "0", "1712487272", "1712487272"}
    };

    storageService.load(init.getDefaultListFilePath());
    storageService.write(defaultListData);

    std::vector <std::vector <std::string>> tempListData = {
            {"aaaa", "test 1", "high", "to-do", "0", "0", "1712487259", "1712487259"},
            {"bbbb", "test 2", "medium", "started", "1712487259", "0", "1712487272", "1712487272"}
    };

    storageService.load(init.getMainDirPath() / "tempListName.json");
    storageService.write(tempListData);

    std::vector <std::vector <std::string>> temp2ListData = {
            {"cccc", "test 1", "high", "to-do", "0", "0", "1712487259", "1712487259"},
            {"dddd", "test 2", "medium", "started", "1712487259", "0", "1712487272", "1712487272"}
    };

    storageService.load(init.getMainDirPath() / "tempList2Name.json");
    storageService.write(temp2ListData);
}

void MockInstallation::createDirectories()
{
    ioService.print(init.getHomeDir().string());
    if (!std::filesystem::exists(init.getMainDirPath())) {
        if (!std::filesystem::create_directory(init.getMainDirPath())) {
            ioService.print("Failed to create application directory");
            return;
        }
    }

    if (!std::filesystem::exists(init.getConfigDirPath())) {
        if (!std::filesystem::create_directory(init.getConfigDirPath())) {
            ioService.print("Failed to create config directory");
            return;
        }
    }

    if (!std::filesystem::exists(init.getAppDirPath())) {
        if (!std::filesystem::create_directory(init.getAppDirPath())) {
            ioService.print("Failed to create application directory");
            return;
        }
    }
}

void MockInstallation::createConfigFile()
{
    std::ofstream outfile(init.getConfigFilePath(), std::ios::out | std::ios::app);
    if (!outfile.is_open())
    {
        ioService.print("Failed to create application config file");
        return;
    }
    outfile.close();
}

void MockInstallation::createListOfListFile()
{
    std::ofstream outfile(init.getListOfListFilePath(), std::ios::out | std::ios::app);
    if (!outfile.is_open())
    {
        ioService.print("Failed to create application list of list file");
        return;
    }
    outfile.close();
}

void MockInstallation::createListFile(const std::string& listName)
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
    for (const auto& filePath : filePaths)
    {
        std::ofstream outfile(filePath, std::ios::out | std::ios::app);
        if (!outfile.is_open())
        {
            ioService.print("Failed to create default application file");
            return;
        }
        populateNewListFile(outfile);
        outfile.close();
    }
}

bool MockInstallation::wipe() {
    try {
        std::filesystem::remove_all(init.getMainDirPath());
    } catch (std::filesystem::filesystem_error& e) {
        ioService.print("Failed to wipe application directory");
        return false;
    }
    return true;
}

void MockInstallation::populateNewListFile(std::ofstream& outfile)
{
    if (init.getDefaultExtension() == "json")
    {
        outfile << "[\n";
        outfile << "]";
    }
    else if (init.getDefaultExtension() == "csv")
    {
        outfile << "# ID,VALUE,PRIORITY,STATUS,DUE_AT,CLOSED_AT,CREATED_AT,UPDATED_AT";
    }
}
