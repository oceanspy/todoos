#include "CLIController.h"
#include "Config/Config.h"
#include "ListActions/ListActions.h"
#include "ListItemActions/ListItemActions.h"
#include "Move/Move.h"
#include "Priority/Priority.h"
#include "Remove/Remove.h"
#include "Show/Show.h"
#include "Stats/Stats.h"
#include "Status/Status.h"
#include <exception>

CLIController::CLIController(IOService& ioService,
                             Help& help,
                             CommandService& commandService,
                             Command& command,
                             ConfigService& configService,
                             FileStorageService& fileStorageService,
                             ListService& listService,
                             ListItemService& listItemService,
                             CLIThemeService& cliThemeService)
  : ioService(ioService)
  , help(help)
  , commandService(commandService)
  , command(command)
  , configService(configService)
  , fileStorageService(fileStorageService)
  , listService(listService)
  , listItemService(listItemService)
  , cliThemeService(cliThemeService)
{
}

void
CLIController::actions()
{
    std::string commandName = CommandService::getCommandName(command.getName());
    std::map<std::string, std::string> options = command.getOptions();

    if (!CommandService::isCommandValidWithOption(CommandService::getCommandName(command.getName()),
                                                  command.getOptions())) {
        help.commandOptionNotSupported();
        return;
    }

    if (!commandService.isValid(commandName)) {
        help.commandNotFound();
        return;
    }

    // TODO: implement shortcuts
    switch (commandService.list().getCommandId(commandName)) {
        case CommandList::SHOW:
            show();
            return;
        case CommandList::ADD:
        case CommandList::EDIT:
        case CommandList::APPEND:
        case CommandList::PREPEND:
        case CommandList::DEADLINE:
            listItemActions();
            return;
        case CommandList::FIND:
            find();
            return;
        case CommandList::PRIORITY:
            priority();
            return;
        case CommandList::INCREASE:
            priority("increase");
            return;
        case CommandList::DECREASE:
            priority("decrease");
            return;
        case CommandList::STATUS:
            status();
            return;
        case CommandList::TO_DO:
            status(StatusService::TO_DO);
            return;
        case CommandList::START:
            status(StatusService::STARTED);
            return;
        case CommandList::REVIEW:
            status(StatusService::REVIEWING);
            return;
        case CommandList::PAUSE:
            status(StatusService::PAUSED);
            return;
        case CommandList::BLOCKED:
            status(StatusService::BLOCKED);
            return;
        case CommandList::COMPLETE:
            status(StatusService::COMPLETED);
            return;
        case CommandList::CANCEL:
            status(StatusService::CANCELLED);
            return;
        case CommandList::REMOVE:
            remove();
            return;
        case CommandList::ARCHIVE:
            archive();
            return;
        case CommandList::RESTORE:
            restore();
            return;
        case CommandList::MOVE:
        case CommandList::COPY:
        case CommandList::DUPLICATE:
            move();
            return;
        case CommandList::EMPTY:
            empty();
            return;
        case CommandList::CLEAN:
            clean();
            return;
        case CommandList::LIST:
            list();
            return;
        case CommandList::USE:
            use();
            return;
        case CommandList::STATS:
            stats();
            return;
        case CommandList::RESET:
            reset();
            return;
        default:
            help.commandNotFound();
            break;
    }
}

void
CLIController::filterListItemsWithOptions(std::vector<ListItemEntity>* listItems)
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
CLIController::show()
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

        Show show(ioService, listService, listItemService, cliThemeService);
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

        Show show(ioService, listService, listItemService, cliThemeService);

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
        ioService.message(Help::getVersion());
        return;
    } else if (command.hasOption("config")) {
        Config config(ioService, command, commandService, configService, listService);

        config.make();
        return;
    }

    std::string listNameStr = configService.getUsedListNameStr();
    ListName listName = listService.createListName(listNameStr, configService.getUsedListVariantStr());
    Show show(ioService, listService, listItemService, cliThemeService);

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

void
CLIController::listItemActions()
{
    ListName listName =
        listService.createListName(configService.getUsedListNameStr(), configService.getUsedListVariantStr());
    ListItemActions listItemActions(ioService, command, commandService, listItemService);
    listItemActions.make(listName);

    Show show(ioService, listService, listItemService, cliThemeService);

    std::vector<ListItemEntity> listItems = listItemService.get(listName);
    try {
        show.print(listItems, listName);
    } catch (std::exception& e) {
        ioService.br();
        ioService.error(e.what());
        ioService.br();
    }
}

void
CLIController::find()
{
    ListName listName =
        listService.createListName(configService.getUsedListNameStr(), configService.getUsedListVariantStr());
    ListName listNameArchive = ListName::createVariant(listName, "archive");
    ListName listNameDelete = ListName::createVariant(listName, "delete");
    Show show(ioService, listService, listItemService, cliThemeService);

    std::string stringSearch;
    for (const std::string& argument : command.getArguments()) {
        stringSearch += argument + " ";
    }

    try {
        std::vector<ListItemEntity> listItems;
        if (command.hasOption("archive")) {
            listItems = listItemService.search(listNameArchive, command.getArguments());

            ioService.br();
            ioService.info("Searching for " + stringSearch + "in archived items...");
            ioService.br();

            show.print(listItems, listName);
        } else if (command.hasOption("delete")) {
            listItems = listItemService.search(listNameDelete, command.getArguments());

            ioService.br();
            ioService.info("Searching for " + stringSearch + "in deleted items...");
            ioService.br();

            try {
                show.print(listItems, listName);
            } catch (std::exception& e) {
                ioService.br();
                ioService.error(e.what());
                ioService.br();
            }
        } else {
            listItems = listItemService.search(listName, command.getArguments());
            ioService.br();
            ioService.info("Searching for " + stringSearch + "...");
            ioService.br();
            try {
                show.print(listItems, listName);
            } catch (std::exception& e) {
                ioService.br();
                ioService.error(e.what());
                ioService.br();
            }
        }
    } catch (std::exception& e) {
        ioService.br();
        ioService.info("Searching for " + stringSearch + "...");
        ioService.br();
        ioService.error("No item found.");
    }
}

void
CLIController::priority(std::string action)
{
    ListName listName =
        listService.createListName(configService.getUsedListNameStr(), configService.getUsedListVariantStr());
    Priority priority(ioService, command, listItemService);

    if (action == "set") {
        priority.set(listName);
    } else if (action == "increase") {
        priority.increase(listName);
    } else if (action == "decrease") {
        priority.decrease(listName);
    }

    Show show(ioService, listService, listItemService, cliThemeService);

    std::vector<ListItemEntity> listItems = listItemService.get(listName);
    try {
        show.print(listItems, listName);
    } catch (std::exception& e) {
        ioService.br();
        ioService.error(e.what());
        ioService.br();
    }
}

void
CLIController::reset()
{
    std::string answer = ioService.ask("Are you sure you want to reset item(s) to brand new and not mark them "
                                       "as to-do? (y/n) ");

    ListName listName =
        listService.createListName(configService.getUsedListNameStr(), configService.getUsedListVariantStr());
    if (answer == "y" || answer == "yes") {
        Status status(ioService, command, listItemService);
        status.reset(listName);

        Show show(ioService, listService, listItemService, cliThemeService);

        std::vector<ListItemEntity> listItems = listItemService.get(listName);
        try {
            show.print(listItems, listName);
        } catch (std::exception& e) {
            ioService.br();
            ioService.error(e.what());
            ioService.br();
        }
    } else {
        ioService.br();
        ioService.message("Action cancelled.");
        ioService.br();
    }
}

void
CLIController::status(int statusNumber)
{
    ListName listName =
        listService.createListName(configService.getUsedListNameStr(), configService.getUsedListVariantStr());
    Status status(ioService, command, listItemService);
    if (statusNumber == -1) {
        status.set(listName);
    } else {
        status.markAs(listName, statusNumber);
    }

    Show show(ioService, listService, listItemService, cliThemeService);

    std::vector<ListItemEntity> listItems = listItemService.get(listName);
    try {
        show.print(listItems, listName);
    } catch (std::exception& e) {
        ioService.br();
        ioService.error(e.what());
        ioService.br();
    }
}

void
CLIController::remove()
{
    ListName listName =
        listService.createListName(configService.getUsedListNameStr(), configService.getUsedListVariantStr());
    Remove remove(ioService, command, listItemService);
    remove.remove(listName);

    Show show(ioService, listService, listItemService, cliThemeService);

    std::vector<ListItemEntity> listItems = listItemService.get(listName);
    try {
        show.print(listItems, listName);
    } catch (std::exception& e) {
        ioService.br();
        ioService.error(e.what());
        ioService.br();
    }
}

void
CLIController::archive()
{
    ListName listName =
        listService.createListName(configService.getUsedListNameStr(), configService.getUsedListVariantStr());
    Remove remove(ioService, command, listItemService);
    remove.archive(listName);

    Show show(ioService, listService, listItemService, cliThemeService);

    std::vector<ListItemEntity> listItems = listItemService.get(listName);
    try {
        show.print(listItems, listName);
    } catch (std::exception& e) {
        ioService.br();
        ioService.error(e.what());
        ioService.br();
    }
}

void
CLIController::restore()
{
    ListName listName =
        listService.createListName(configService.getUsedListNameStr(), configService.getUsedListVariantStr());
    Remove remove(ioService, command, listItemService);
    remove.restore(listName);

    Show show(ioService, listService, listItemService, cliThemeService);

    std::vector<ListItemEntity> listItems = listItemService.get(listName);
    try {
        show.print(listItems, listName);
    } catch (std::exception& e) {
        ioService.br();
        ioService.error(e.what());
        ioService.br();
    }
}

void
CLIController::list()
{
    ListActions list(ioService,
                     command,
                     commandService,
                     listService,
                     listItemService,
                     fileStorageService,
                     configService,
                     cliThemeService);
    list.make();
}

void
CLIController::use()
{
    ListActions list(ioService,
                     command,
                     commandService,
                     listService,
                     listItemService,
                     fileStorageService,
                     configService,
                     cliThemeService);

    list.use();

    ListName listName =
        listService.createListName(configService.getUsedListNameStr(), configService.getUsedListVariantStr());
    Show show(ioService, listService, listItemService, cliThemeService);

    std::vector<ListItemEntity> listItems = listItemService.get(listName);

    try {
        show.print(listItems, listName);
    } catch (std::exception& e) {
        ioService.br();
        ioService.error(e.what());
        ioService.br();
    }
}

void
CLIController::move()
{
    Move move(ioService, command, commandService, listService, listItemService);

    ListName listName =
        listService.createListName(configService.getUsedListNameStr(), configService.getUsedListVariantStr());
    std::string newListNameStr = move.make(listName);
    if (!newListNameStr.empty()) {

        ListName newListName = listService.createListName(newListNameStr, configService.getUsedListVariantStr());
        Show show(ioService, listService, listItemService, cliThemeService);

        std::vector<ListItemEntity> listItems = listItemService.get(newListName);
        try {
            show.print(listItems, newListName);
        } catch (std::exception& e) {
            ioService.br();
            ioService.error(e.what());
            ioService.br();
        }
    }
}

void
CLIController::empty()
{
    ioService.br();
    std::string message = "Are you sure you want to empty ";
    message += StringHelpers::colorize(configService.getUsedListNameStr(), RED);
    message += " list? (yes/no) ";
    std::string answer = ioService.ask(message);

    if (answer == "yes") {
        ListName listName =
            listService.createListName(configService.getUsedListNameStr(), configService.getUsedListVariantStr());
        listItemService.archiveAll(listName);
        ioService.br();
        ioService.success("List cleared.");
        ioService.br();
    } else {
        ioService.br();
        ioService.message("Action cancelled.");
        ioService.br();
    }
}

void
CLIController::clean()
{
    ioService.br();
    std::string message = "Are you sure you want to archive all completed & cancelled from ";
    message += StringHelpers::colorize(configService.getUsedListNameStr(), RED);
    message += " list? (yes/no) ";
    std::string answer = ioService.ask(message);

    if (answer == "yes") {
        ListName listName =
            listService.createListName(configService.getUsedListNameStr(), configService.getUsedListVariantStr());
        listItemService.archiveFinishedItems(listName);
        ioService.br();
        ioService.success("List cleaned.");
        ioService.br();

        Show show(ioService, listService, listItemService, cliThemeService);

        std::vector<ListItemEntity> listItems = listItemService.get(listName);

        try {
            show.print(listItems, listName);
        } catch (std::exception& e) {
            ioService.br();
            ioService.error(e.what());
            ioService.br();
        }
    } else {
        ioService.br();
        ioService.message("Action cancelled.");
        ioService.br();
    }
}

void
CLIController::stats()
{
    ListName listName =
        listService.createListName(configService.getUsedListNameStr(), configService.getUsedListVariantStr());
    Stats stats(ioService, configService, command, listItemService, cliThemeService, listName);

    stats.print();
}
