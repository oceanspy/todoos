#ifndef COMMANDROUTER_H
#define COMMANDROUTER_H

#include <filesystem>

#include "../Command/Command.h"
#include "../Command/CommandService.h"
#include "../Command/SmartCommand.h"
#include "../Config/ConfigService.h"
#include "../FileStorage/FileStorageService.h"
#include "../Help/Help.h"
#include "../IOService/IOService.h"
#include "../List/ListItemService.h"
#include "../List/ListName.h"
#include "../List/ListService.h"
#include "../Themes/ThemeService.h"

class CommandRouter
{
  public:
    CommandRouter(IOService& ioService,
                  Help& help,
                  CommandService& commandService,
                  ConfigService& configService,
                  FileStorageService& fileStorageService,
                  ListService& listService,
                  ListItemService& listItemService,
                  ThemeService& themeService);

    void execute(Command& command);

  private:
    IOService& ioService;
    Help& help;
    CommandService& commandService;
    ConfigService& configService;
    std::filesystem::path currentListFilePath;
    FileStorageService& fileStorageService;
    ListService& listService;
    ListItemService& listItemService;
    ThemeService& themeService;
};

#endif // COMMANDROUTER_H
