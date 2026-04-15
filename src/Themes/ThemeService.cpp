#include "ThemeService.h"
#include "Presets/Default.h"
#include "Presets/Legacy.h"

ThemeService::ThemeService(IOService& ioService,
                           ConfigService& configService,
                           ListService& listService,
                           ListItemService& listItemService)
  : ioService(ioService)
  , configService(configService)
  , listService(listService)
  , listItemService(listItemService)
{
    // Get console width
    consoleWidth = IOService::getConsoleDisplayWidth();
    getConsoleRowMaxLengthAndThemeType();
}

std::unique_ptr<Theme>
ThemeService::getTheme()
{
    std::string themeName = configService.getValue("theme");

    if (themeName.length() <= 0) {
        return std::make_unique<Default>(ioService, listService, listItemService, consoleWidth, consoleRowLength);
    }

    if (themeName == "default") {
        return std::make_unique<Default>(ioService, listService, listItemService, consoleWidth, consoleRowLength);
    } else if (themeName == "legacy") {
        return std::make_unique<Legacy>(ioService, listService, listItemService, consoleWidth, consoleRowLength);
    }

    // Unable to find the theme defined, we show an error and use the default
    ioService.br();
    ioService.error("Theme " + themeName + " not found. Using default.");
    ioService.br();
    return std::make_unique<Default>(ioService, listService, listItemService, consoleWidth, consoleRowLength);
}

void
ThemeService::getConsoleRowMaxLengthAndThemeType()
{
    consoleRowLength = CONST_WIDTH_DEFAULT_VALUE;

    // No console width set, we take default
    if (consoleWidth == 0) {
        consoleWidth = CONST_WIDTH_DEFAULT_VALUE;
    }

    // check if user has set a custom consoleRowMaxLength -- otherwise we take full width
    consoleRowLength = !configService.getValue("consoleRowMaxLength").empty() &&
                               std::stoi(configService.getValue("consoleRowMaxLength")) > 0
                           ? std::stoi(configService.getValue("consoleRowMaxLength"))
                           : consoleWidth;

    if (consoleRowLength > consoleWidth) {
        consoleRowLength = consoleWidth;
    }

    // consoleRowMaxLength have to be even:
    if (consoleRowLength % 2 != 0) {
        consoleRowLength -= 1;
    }
}

ThemeService
ThemeService::adaptConsoleRowLengthWithMaxItemValueLength(const std::vector<ListItemEntity>& listItems)
{
    getConsoleRowMaxLengthAndThemeType();

    int rowValueMaxLength = 0;
    for (const auto& listItem : listItems) {
        if ((*listItem.getValue()).length() > rowValueMaxLength) {
            rowValueMaxLength = static_cast<int>((*listItem.getValue()).length());
        }
    }

    // row max length is short => we keep the list short
    if (rowValueMaxLength <= 50) {
        consoleRowLength = CONST_WIDTH_DEFAULT_VALUE > consoleWidth ? consoleWidth : CONST_WIDTH_DEFAULT_VALUE;
        // Title max length is high, we use the default wide value
    } else {
        consoleRowLength =
            CONST_WIDTH_DEFAULT_WIDE_VALUE > consoleWidth ? consoleWidth : CONST_WIDTH_DEFAULT_WIDE_VALUE;
    }
    return *this;
}

ThemeService
ThemeService::substractConsoleRowLength(const int substract)
{
    getConsoleRowMaxLengthAndThemeType();

    if (consoleRowLength > 0 && consoleRowLength - substract > 0) {
        consoleRowLength -= substract;
    }

    return *this;
}
