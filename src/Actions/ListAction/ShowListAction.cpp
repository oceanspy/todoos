#include "ShowListAction.h"
#include "../../Helpers/StringHelpers.h"
#include "../../Helpers/TerminalStyle.h"
#include "../../Themes/Theme.h"

ShowListAction::ShowListAction(IOService& ioService,
                               ListService& listService,
                               ConfigService& configService,
                               ThemeService& themeService)
  : ioService(ioService)
  , listService(listService)
  , configService(configService)
  , themeService(themeService)
{
}

void
ShowListAction::execute(Command& command)
{
    auto theme = themeService.getTheme();
    ioService.brOrSkip();
    ioService.title("Lists available");

    std::vector<ListEntity> listItems = listService.get();
    int i = 0;
    std::vector<std::string> lines;
    for (ListEntity& listItem : listItems) {
        std::string line;
        ++i;
        line = " " + std::to_string(i) + ". ";
        line += buildName(listItem);
        lines.push_back(line);
    }

    theme->printFullLine(GRAY);
    theme->printAList(lines);
}

std::string
ShowListAction::buildName(const ListEntity& listEntity)
{
    std::string name = addSpaceToListName(*listEntity.getName());

    if (configService.getDefaultList() == *listEntity.getName()) {
        name += " (default)";
    }

    if (configService.getUsedListNameStr() == *listEntity.getName()) {
        name += " (in use)";
        name = StringHelpers::colorize(name, GREEN);
    } else if (configService.getDefaultList() == *listEntity.getName()) {
        name = StringHelpers::colorize(name, BLUE);
    }

    return StringHelpers::adjustStringLength(name, 40);
}

std::string
ShowListAction::addSpaceToListName(std::string listName)
{
    std::replace(listName.begin(), listName.end(), '_', ' ');
    return listName;
}
