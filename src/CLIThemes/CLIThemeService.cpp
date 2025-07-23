#include "CLIThemeService.h"
#include "DefaultTheme.h"
#include "MobileTheme.h"

CLIThemeService::CLIThemeService(IOService& ioService,
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
    typeOfTheme = "default";
    getConsoleRowMaxLengthAndThemeType();
}

ThemeAbstract*
CLIThemeService::getTheme()
{
    std::string theme = configService.getValue("theme");
    if (theme == "default" && typeOfTheme == "mobile") {
        return new MobileTheme(ioService, listService, listItemService, consoleWidth, consoleRowLength);
    } else if (theme == "default" && typeOfTheme == "default") {
        return new DefaultTheme(ioService, listService, listItemService, consoleWidth, consoleRowLength);
    }

    throw std::invalid_argument("Theme not found -- Please fix your config file. [default theme: default]");
}

ThemeAbstract*
CLIThemeService::getLightTheme()
{
    return new MobileTheme(ioService, listService, listItemService, consoleWidth, consoleRowLength);
}
void
CLIThemeService::getConsoleRowMaxLengthAndThemeType()
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

    // If console width is too small => mobile version
    if (consoleRowLength <= 120) {
        typeOfTheme = "mobile";
    }

    // consoleRowMaxLength have to be even:
    if (consoleRowLength % 2 != 0) {
        consoleRowLength -= 1;
    }
}

CLIThemeService
CLIThemeService::adaptConsoleRowLengthWithMaxItemValueLength(const std::vector<ListItemEntity>& listItems)
{
    getConsoleRowMaxLengthAndThemeType();

    int titleMaxLength = 0;
    for (const auto& listItem : listItems) {
        if ((*listItem.getValue()).length() > titleMaxLength) {
            titleMaxLength = static_cast<int>((*listItem.getValue()).length());
        }
    }

    if (titleMaxLength <= 60) {
        consoleRowLength = CONST_WIDTH_DEFAULT_VALUE > consoleWidth ? consoleWidth : CONST_WIDTH_DEFAULT_VALUE;
        return *this;
    }

    if (consoleRowLength > 150 && titleMaxLength + 82 < consoleRowLength) {
        if (titleMaxLength <= 80) {
            consoleRowLength = titleMaxLength + 82;
        } else {
            consoleRowLength = titleMaxLength + 72;
        }

        if (consoleRowLength % 2 != 0) {
            consoleRowLength += 1;
        }
    }

    return *this;
}

CLIThemeService
CLIThemeService::substractConsoleRowLength(const int substract)
{
    getConsoleRowMaxLengthAndThemeType();

    if (consoleRowLength > 0 && consoleRowLength - substract > 0) {
        consoleRowLength -= substract;
    }

    return *this;
}
