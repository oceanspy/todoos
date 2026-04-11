#ifndef TODOOS_THEMESERVICE_H
#define TODOOS_THEMESERVICE_H

#include "../Config/ConfigService.h"
#include "ThemeAbstract.h"
#include <memory>
#include <vector>

#define CONST_WIDTH_DEFAULT_VALUE 140

class ThemeService
{
  public:
    explicit ThemeService(IOService& ioService,
                          ConfigService& configService,
                          ListService& listService,
                          ListItemService& listItemService);
    void getConsoleRowMaxLengthAndThemeType();
    ThemeService adaptConsoleRowLengthWithMaxItemValueLength(const std::vector<ListItemEntity>& listItems);
    ThemeService substractConsoleRowLength(const int substract = 0);
    std::unique_ptr<ThemeAbstract> getTheme();

  private:
    IOService& ioService;
    ConfigService& configService;
    ListService& listService;
    ListItemService& listItemService;
    std::string typeOfTheme;
    int consoleWidth;
    int consoleRowLength;
};

#endif // TODOOS_THEMESERVICE_H
