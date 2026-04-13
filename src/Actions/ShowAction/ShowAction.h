#ifndef SHOW_H
#define SHOW_H
#include "../../IOService/IOService.h"
#include "../../List/ListItemService.h"
#include "../../List/ListName.h"
#include "../../List/ListService.h"
#include "../../Themes/ThemeService.h"

class ShowAction
{
  public:
    ShowAction(IOService& ioService,
               ListService& listService,
               ListItemService& listItemService,
               ThemeService& themeService);
    void execute(std::vector<ListItemEntity>& listItems,
                 ListName& listName,
                 bool showListName = true,
                 bool showTitle = true);
    void executeMultipleList(std::vector<ListItemEntity>& listItems, std::vector<ListName>& listNames);

  private:
    IOService& ioService;
    ListService& listService;
    ListItemService& listItemService;
    ThemeService& themeService;
};

#endif // SHOW_H
