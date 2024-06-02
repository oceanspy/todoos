#include "Installation.h"

Installation::Installation(IOService& ioService,
                           FileDataServiceInterface& storageJsonService,
                           FileDataServiceInterface& storageCsvService,
                           FileDataServiceInterface& configStorageService,
                           InitInterface& init
                           ) :
                           ioService(ioService),
                           storageJsonService(storageJsonService),
                           storageCsvService(storageCsvService),
                           configStorageService(configStorageService),
                           init(init)
{

}

bool Installation::isNew()
{
    return !std::filesystem::exists(init.getConfigFilePath());
}

void Installation::make()
{
    ioService.info("First launch detected.");
    ioService.message("Inializing program...");
    this->createDirectories();
    this->createConfigFile();
    this->createListOfListFile();
    this->createListFile(init.getDefaultListFileName());
    this->populate();
    ioService.success("Program initialized.");
    ioService.message("You can adapt your settings in " + init.getConfigFilePath().string());
    ioService.info("Welcome! :-)");
}

void Installation::populate()
{
    std::vector <std::vector <std::string>> defaultConfigData = {
            { "appDirStorage", init.getAppDirPath().string() },
            { "fileDataStorageType", "csv" },
            { "currentList", "default" },
            { "defaultList", "default" },
            { "theme", "default" },
            { "consoleRowMaxLength", "0" },
            { "archiveWhenCompleted", "false" },
            { "useOnlyLettersForIds", "true" }
    };

    configStorageService.load(init.getConfigFilePath());
    configStorageService.write(defaultConfigData);

    std::vector <std::vector <std::string>> defaultListOfListData = {
            { "default", "default", "default", "false" }
    };

    storageJsonService.load(init.getListOfListFilePath());
    storageJsonService.write(defaultListOfListData);

    std::vector <std::vector <std::string>> defaultListData = {
            {"aaaa", "Welcome to todoos!", "urgent", "to-do", "0", "0", std::to_string(time(nullptr)), std::to_string(time(nullptr))},
            {"bbbb", "Please look at 'todoos -h' to get started!", "high", "to-do", "0", "0", std::to_string(time(nullptr)), std::to_string(time(nullptr))},
            {"cccc", "Add your first task by doing: 'todoos add \"My first task\"'", "medium", "to-do", "0", "0", std::to_string(time(nullptr)), std::to_string(time(nullptr))},
            {"dddd", "Enjoy! :-)", "low", "to-do", "0", "0", std::to_string(time(nullptr)), std::to_string(time(nullptr))}
    };

    storageCsvService.load(init.getDefaultListFilePath());
    storageCsvService.write(defaultListData);
}

void Installation::createDirectories()
{
    if (!std::filesystem::exists(init.getMainDirPath())) {
        if (!std::filesystem::create_directory(init.getMainDirPath())) {
            ioService.error("Failed to create application directory");
            return;
        }
    }

    if (!std::filesystem::exists(init.getConfigDirPath())) {
        if (!std::filesystem::create_directory(init.getConfigDirPath())) {
            ioService.error("Failed to create config directory");
            return;
        }
    }

    if (!std::filesystem::exists(init.getAppDirPath())) {
        if (!std::filesystem::create_directory(init.getAppDirPath())) {
            ioService.error("Failed to create application directory");
            return;
        }
    }
}

void Installation::createConfigFile()
{
    std::ofstream outfile(init.getConfigFilePath(), std::ios::out | std::ios::app);
    if (!outfile.is_open())
    {
        ioService.error("Failed to create application config file");
        return;
    }
    outfile.close();
}

void Installation::createListOfListFile()
{
    std::ofstream outfile(init.getListOfListFilePath(), std::ios::out | std::ios::app);
    if (!outfile.is_open())
    {
        ioService.error("Failed to create application list of list file");
        return;
    }
    outfile.close();
}

void Installation::createListFile(const std::string& listName)
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

bool Installation::wipe() {
    // TODO: Implement wipe method
    return false;
}

void Installation::populateNewListFile(std::ofstream& outfile)
{
    if (init.getDefaultExtension() == "json")
    {
        outfile << "[\n";
        outfile << "]";
    }
    else if (init.getDefaultExtension() == "csv")
    {
        outfile << "# ID,VALUE,PRIORITY,STATUS,DUE_AT,CLOSED_AT,CREATED_AT,UPDATED_AT\n";
    }
}
