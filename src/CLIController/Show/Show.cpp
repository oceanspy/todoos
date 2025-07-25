#include "Show.h"

Show::Show(IOService& ioService,
           ConfigService& configService,
           ListService& listService,
           ListItemService& listItemService,
           CLIThemeService& cliThemeService)
  : ioService(ioService)
  , configService(configService)
  , listService(listService)
  , listItemService(listItemService)
  , cliThemeService(cliThemeService)
{
}

void
Show::print(std::vector<ListItemEntity>& listItems,
            std::string& listName,
            std::string& listVariant,
            bool showListName,
            bool showTitle)
{
    ThemeAbstract* theme = cliThemeService.adaptConsoleRowLengthWithMaxItemValueLength(listItems).getTheme();
    theme->print(listName, listVariant, listItems, showListName, showTitle);
}

void
Show::printMultipleList(std::vector<ListItemEntity>& listItems,
                        std::vector<std::string>& listNames,
                        std::string& listVariant)
{
    ThemeAbstract* theme = cliThemeService.adaptConsoleRowLengthWithMaxItemValueLength(listItems).getTheme();
    theme->printMultipleList(listNames, listVariant, listItems);
}
