#ifndef SHOW_H
#define SHOW_H
#include "../../CLIThemes/CLIThemeService.h"
#include "../../IOService/IOService.h"
#include "../../List/ListItemService.h"
#include "../../List/ListName.h"
#include "../../List/ListService.h"

class Show
{
  public:
    Show(IOService& ioService,
         ListService& listService,
         ListItemService& listItemService,
         CLIThemeService& cliThemeService);
    void print(std::vector<ListItemEntity>& listItems,
               ListName& listName,
               bool showListName = true,
               bool showTitle = true);
    void printMultipleList(std::vector<ListItemEntity>& listItems, std::vector<ListName>& listNames);

  private:
    IOService& ioService;
    ListService& listService;
    ListItemService& listItemService;
    CLIThemeService& cliThemeService;
};

#endif // SHOW_H
