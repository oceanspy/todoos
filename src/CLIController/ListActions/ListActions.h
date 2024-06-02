#ifndef LISTACTIONS_H
#define LISTACTIONS_H

#include "../../List/ListService.h"
#include "../../IOService/IOService.h"
#include "../../Command/Command.h"
#include "../../Command/CommandService.h"
#include "../../List/ListItemService.h"
#include "../../FileStorage/FileStorageService.h"
#include "../../CLIThemes/CLIThemeService.h"

class ListActions {
public:
    ListActions(IOService& ioService,
                Command& command,
                CommandService& commandService,
                ListService& listService,
                ListItemService& listItemService,
                FileStorageService& fileStorageService,
                ConfigService& configService,
                CLIThemeService& cliThemeService
                );
    void make();
    void use();
private:
    IOService& ioService;
    Command& command;
    CommandService& commandService;
    ListService& listService;
    ListItemService& listItemService;
    FileStorageService& fileStorageService;
    ConfigService& configService;
    CLIThemeService& cliThemeService;
    void showList();
    void addList(Command subCommand);
    void removeList(Command subCommand);
    void renameList(Command subCommand);
    std::string buildName(const ListEntity& listEntity);
    void copy();
    static std::string makeListNameFromArguments(std::vector<std::string> &arguments) ;
    static std::string removeSpaceFromListName(std::string listName);
    static std::string addSpaceToListName(std::string listName);
};



#endif //LISTACTIONS_H
