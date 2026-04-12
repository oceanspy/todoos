#include "ThemeService.h"
#include "Default/Default.h"

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

ThemeService
ThemeService::substractConsoleRowLength(const int substract)
{
    getConsoleRowMaxLengthAndThemeType();

    if (consoleRowLength > 0 && consoleRowLength - substract > 0) {
        consoleRowLength -= substract;
    }

    return *this;
}
