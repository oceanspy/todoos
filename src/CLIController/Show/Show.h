#ifndef SHOW_H
#define SHOW_H
#include "../../CLIThemes/CLIThemeService.h"
#include "../../IOService/IOService.h"
#include "../../List/ListItemService.h"
#include "../../List/ListService.h"

class Show
{
  public:
    Show(IOService& ioService,
         ConfigService& configService,
         ListService& listService,
         ListItemService& listItemService,
         CLIThemeService& cliThemeService);
    void print(std::vector<ListItemEntity>& listItems,
               std::string& listName,
               std::string& listVariant,
               bool showListName = true,
               bool showTitle = true);
    void printMultipleList(std::vector<ListItemEntity>& listItems,
                           std::vector<std::string>& listNames,
                           std::string& listVariant);

  private:
    IOService& ioService;
    ConfigService& configService;
    ListService& listService;
    ListItemService& listItemService;
    CLIThemeService& cliThemeService;
};

#endif // SHOW_H
