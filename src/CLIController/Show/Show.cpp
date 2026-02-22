#include "Show.h"

Show::Show(IOService& ioService,
           ListService& listService,
           ListItemService& listItemService,
           CLIThemeService& cliThemeService)
  : ioService(ioService)
  , listService(listService)
  , listItemService(listItemService)
  , cliThemeService(cliThemeService)
{
}

void
Show::print(std::vector<ListItemEntity>& listItems, ListName& listName, bool showListName, bool showTitle)
{
    ThemeAbstract* theme = cliThemeService.adaptConsoleRowLengthWithMaxItemValueLength(listItems).getTheme();
    theme->print(listName, listItems, showListName, showTitle);
}

void
Show::printMultipleList(std::vector<ListItemEntity>& listItems, std::vector<ListName>& listNames)
{
    ThemeAbstract* theme = cliThemeService.adaptConsoleRowLengthWithMaxItemValueLength(listItems).getTheme();
    theme->printMultipleList(listNames, listItems);
}
