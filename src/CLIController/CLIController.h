#ifndef CLISERVICE_H
#define CLISERVICE_H

#include <filesystem>

#include "../CLIThemes/CLIThemeService.h"
#include "../Command/Command.h"
#include "../Command/CommandService.h"
#include "../Command/SmartCommand.h"
#include "../Config/ConfigService.h"
#include "../FileStorage/FileStorageService.h"
#include "../Help/Help.h"
#include "../IOService/IOService.h"
#include "../List/ListItemService.h"
#include "../List/ListService.h"

class CLIController
{
  public:
    CLIController(IOService& ioService,
                  Help& help,
                  CommandService& commandService,
                  Command& command,
                  ConfigService& configService,
                  FileStorageService& fileStorageService,
                  ListService& listService,
                  ListItemService& listItemService,
                  CLIThemeService& cliThemeService);

    void actions();

  private:
    IOService& ioService;
    Help& help;
    CommandService& commandService;
    Command& command;
    ConfigService& configService;
    std::filesystem::path currentListFilePath;
    FileStorageService& fileStorageService;
    ListService& listService;
    ListItemService& listItemService;
    CLIThemeService& cliThemeService;
    std::string currentList = configService.getCurrentList();
    std::string currentListVariant = configService.getCurrentListVariant();
    void show();
    void listItemActions();
    void find();
    void priority(std::string action = "set");
    void reset();
    void status(int statusNumber = -1);
    void remove();
    void archive();
    void restore();
    void list();
    void use();
    void stats();
    void move();
    void empty();
    void clean();
};

#endif // CLISERVICE_H
