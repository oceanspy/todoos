#include "ShowUseCase.h"
#include "../Actions/ConfigAction/ConfigAction.h"
#include "../Actions/ShowAction/ShowAction.h"
#include "../Helpers/DateHelpers.h"
#include "../List/ListEntity.h"
#include "../List/ListName.h"

ShowUseCase::ShowUseCase(IOService& ioService,
                         HelpPrinter& help,
                         CommandService& commandService,
                         Command& command,
                         ConfigService& configService,
                         ListService& listService,
                         ListItemService& listItemService,
                         ThemeService& themeService)
  : ioService(ioService)
  , help(help)
  , commandService(commandService)
  , command(command)
  , configService(configService)
  , listService(listService)
  , listItemService(listItemService)
  , themeService(themeService)
{
}

void
ShowUseCase::filterListItemsWithOptions(std::vector<ListItemEntity>* listItems)
{
    if (command.hasOption("priority")) {
        try {
            int priority = listItemService.priority().getIdFromName(command.getOption("priority"));
            listItemService.filterPriorityAbove(*listItems, priority);
        } catch (std::invalid_argument& e) {
            ioService.br();
            ioService.error(e.what());
            ioService.br();
            return;
        }
    }
    if (command.hasOption("status")) {
        try {
            int status = listItemService.status().getIdFromName(command.getOption("status"));
            listItemService.filterStatus(*listItems, { status });
        } catch (std::invalid_argument& e) {
            ioService.br();
            ioService.error(e.what());
            ioService.br();
            return;
        }
    }
    if (command.hasOption("deadline")) {
        try {
            listItemService.filterDeadlineBefore(*listItems,
                                                 DateHelpers::relativeDateToTimestamp(command.getOption("deadline")));
            listItemService.filterStatus(*listItems, { 0, 1, 2 });
        } catch (std::invalid_argument& e) {
            ioService.br();
            ioService.error(e.what());
            ioService.br();
            return;
        }
    }
}

void
ShowUseCase::execute()
{
    if (commandService.hasSubCommand(command) && commandService.getSubCommand(command).getName() == "all") {
        std::vector<ListName> listNames = {};
        std::vector<ListItemEntity> allListItems = {};

        std::vector<ListEntity> lists = listService.get();
        for (const ListEntity& list : lists) {
            ListName listName = listService.createListName(*list.getName(), configService.getUsedListVariantStr());
            listNames.push_back(listName);

            std::vector<ListItemEntity> listItems = listItemService.get(listName);
            filterListItemsWithOptions(&listItems);

            for (auto listItem : listItems) {
                allListItems.push_back(listItem);
            }
        }

        ShowAction show(ioService, listService, listItemService, themeService);
        try {
            show.printMultipleList(allListItems, listNames);
        } catch (std::exception& e) {
            ioService.br();
            ioService.error(e.what());
            ioService.br();
        }

        return;
    }

    if (commandService.hasSubCommand(command) && command.countArguments() >= 1) {
        std::string variant = configService.getUsedListVariantStr();
        std::vector<ListName> listNames = {};
        std::vector<ListItemEntity> allListItems = {};

        for (std::string listNameStr : command.getArguments()) {
            if (listNameStr.back() == '*') {
                try {
                    std::vector<ListName> listNamesAutocomplete =
                        listService.getAutocompletedLists(listNameStr, variant);
                    listNames.insert(listNames.end(), listNamesAutocomplete.begin(), listNamesAutocomplete.end());
                } catch (std::exception& e) {
                    // do nothing
                }

                continue;
            }

            if (!listService.isListExist(listNameStr)) {
                help.listNotFound(listNameStr);
                return;
            }

            ListName listName = listService.createListName(listNameStr, variant);
            listNames.push_back(listName);
        }

        for (auto listName : listNames) {
            std::vector<ListItemEntity> listItems = listItemService.get(listName);
            filterListItemsWithOptions(&listItems);

            for (auto listItem : listItems) {
                allListItems.push_back(listItem);
            }
        }

        ShowAction show(ioService, listService, listItemService, themeService);

        try {
            if (listNames.size() > 1) {
                show.printMultipleList(allListItems, listNames);
            } else {
                show.print(allListItems, listNames.at(0));
            }
        } catch (std::exception& e) {
            ioService.br();
            ioService.error(e.what());
            ioService.br();
        }

        return;
    }

    if (command.hasOption("help")) {
        help.show();
        return;
    } else if (command.hasOption("version")) {
        ioService.message(HelpPrinter::getVersion());
        return;
    } else if (command.hasOption("config")) {
        ConfigAction config(ioService, command, commandService, configService, listService);
        config.execute();
        return;
    }

    std::string listNameStr = configService.getUsedListNameStr();
    ListName listName = listService.createListName(listNameStr, configService.getUsedListVariantStr());
    ShowAction show(ioService, listService, listItemService, themeService);

    std::vector<ListItemEntity> listItems = listItemService.get(listName);
    filterListItemsWithOptions(&listItems);

    try {
        show.print(listItems, listName);
    } catch (std::exception& e) {
        ioService.br();
        ioService.error(e.what());
        ioService.br();
    }
}
