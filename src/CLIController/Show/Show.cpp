#include "Show.h"

Show::Show(IOService& ioService,
           ConfigService& configService,
           ListService& listService,
           ListItemService& listItemService,
           CLIThemeService& cliThemeService,
           std::string& currentList,
           std::string& currentListVariant)
  : ioService(ioService)
  , configService(configService)
  , listService(listService)
  , listItemService(listItemService)
  , cliThemeService(cliThemeService)
  , currentList(currentList)
  , currentListVariant(currentListVariant)
{
}

void
Show::print(std::vector<ListItemEntity>& listItems, bool showListName, bool showTitle)
{
    ThemeAbstract* theme = cliThemeService.adaptConsoleRowLengthWithMaxItemValueLength(listItems).getTheme();
    theme->print(currentList, currentListVariant, listItems, showListName, showTitle);
}

void
Show::printMultipleList(std::vector<ListItemEntity>& listItems, std::string& listName, std::string& listVariant)
{
    ThemeAbstract* theme = cliThemeService.adaptConsoleRowLengthWithMaxItemValueLength(listItems).getTheme();
    theme->printMultipleList(listName, listVariant, listItems);
}

void
Show::printInAllLoop(std::vector<ListItemEntity>& listItems, bool showListName, bool showTitle)
{
    ThemeAbstract* theme = cliThemeService.substractConsoleRowLength(15).getLightTheme();
    theme->print(currentList, currentListVariant, listItems, showListName, showTitle);
}
