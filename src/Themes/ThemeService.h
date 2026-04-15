#ifndef TODOOS_THEMESERVICE_H
#define TODOOS_THEMESERVICE_H

#include "../Config/ConfigService.h"
#include "Theme.h"
#include <memory>
#include <vector>

#define CONST_WIDTH_DEFAULT_WIDE_VALUE 160
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
    std::unique_ptr<Theme> getTheme();

  private:
    IOService& ioService;
    ConfigService& configService;
    ListService& listService;
    ListItemService& listItemService;
    int consoleWidth;
    int consoleRowLength;
};

#endif // TODOOS_THEMESERVICE_H
