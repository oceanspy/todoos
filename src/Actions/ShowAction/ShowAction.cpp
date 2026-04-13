#include "ShowAction.h"

ShowAction::ShowAction(IOService& ioService,
                       ListService& listService,
                       ListItemService& listItemService,
                       ThemeService& themeService)
  : ioService(ioService)
  , listService(listService)
  , listItemService(listItemService)
  , themeService(themeService)
{
}

void
ShowAction::execute(std::vector<ListItemEntity>& listItems, ListName& listName, bool showListName, bool showTitle)
{
    auto theme = themeService.adaptConsoleRowLengthWithMaxItemValueLength(listItems).getTheme();
    theme->print(listName, listItems, showListName, showTitle);
}

void
ShowAction::executeMultipleList(std::vector<ListItemEntity>& listItems, std::vector<ListName>& listNames)
{
    auto theme = themeService.adaptConsoleRowLengthWithMaxItemValueLength(listItems).getTheme();
    theme->printMultipleList(listNames, listItems);
}
