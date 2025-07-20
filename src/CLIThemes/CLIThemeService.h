#ifndef TODOOS_CLITHEMESERVICE_H
#define TODOOS_CLITHEMESERVICE_H

#include "../Config/ConfigService.h"
#include "ThemeAbstract.h"
#include <vector>

#define CONST_WIDTH_DEFAULT_VALUE 140

class CLIThemeService
{
  public:
    explicit CLIThemeService(IOService& ioService,
                             ConfigService& configService,
                             ListService& listService,
                             ListItemService& listItemService);
    void getConsoleRowMaxLengthAndThemeType();
    CLIThemeService adaptConsoleRowLengthWithMaxItemValueLength(const std::vector<ListItemEntity>& listItems);
    CLIThemeService substractConsoleRowLength(const int substract = 0);
    ThemeAbstract* getTheme();

  private:
    IOService& ioService;
    ConfigService& configService;
    ListService& listService;
    ListItemService& listItemService;
    std::string typeOfTheme;
    int consoleWidth;
    int consoleRowLength;
};

#endif // TODOOS_CLITHEMESERVICE_H
