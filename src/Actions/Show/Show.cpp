#include "Show.h"

Show::Show(IOService& ioService, ListService& listService, ListItemService& listItemService, ThemeService& themeService)
  : ioService(ioService)
  , listService(listService)
  , listItemService(listItemService)
  , themeService(themeService)
{
}

void
Show::print(std::vector<ListItemEntity>& listItems, ListName& listName, bool showListName, bool showTitle)
{
    auto theme = themeService.adaptConsoleRowLengthWithMaxItemValueLength(listItems).getTheme();
    theme->print(listName, listItems, showListName, showTitle);
}

void
Show::printMultipleList(std::vector<ListItemEntity>& listItems, std::vector<ListName>& listNames)
{
    auto theme = themeService.adaptConsoleRowLengthWithMaxItemValueLength(listItems).getTheme();
    theme->printMultipleList(listNames, listItems);
}
