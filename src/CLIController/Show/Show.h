#ifndef SHOW_H
#define SHOW_H
#include "../../IOService/IOService.h"
#include "../../List/ListItemService.h"
#include "../../List/ListService.h"
#include "../../CLIThemes/CLIThemeService.h"

class Show {
public:
    Show(IOService& ioService, ConfigService& configService, ListService& listService, ListItemService& listItemService, CLIThemeService& cliThemeService, std::string& currentList, std::string& currentListVariant);
    void print(std::vector <ListItemEntity>& listItems, bool showListName = true, bool showTitle = true);
    void printInAllLoop(std::vector <ListItemEntity>& listItems, bool showListName = true, bool showTitle = true);
private:
    IOService& ioService;
    ConfigService& configService;
    ListService& listService;
    ListItemService& listItemService;
    CLIThemeService& cliThemeService;
    std::string& currentList;
    std::string& currentListVariant;
};



#endif //SHOW_H
