#include "CLIController.h"

#include "Show/Show.h"
#include "ListItemActions/ListItemActions.h"
#include "Config/Config.h"
#include "Remove/Remove.h"
#include "Move/Move.h"
#include "Status/Status.h"
#include "Priority/Priority.h"
#include "ListActions/ListActions.h"
#include "Stats/Stats.h"

CLIController::CLIController(IOService& ioService,
                             Help& help,
                             CommandService& commandService,
                             Command& command,
                             ConfigService& configService,
                             FileStorageService& fileStorageService,
                             ListService& listService,
                             ListItemService& listItemService,
                             CLIThemeService& cliThemeService
                        ) :
        ioService(ioService),
        help(help),
        commandService(commandService),
        command(command),
        configService(configService),
        fileStorageService(fileStorageService),
        listService(listService),
        listItemService(listItemService),
        cliThemeService(cliThemeService)
{

}

void CLIController::actions()
{
    std::string commandName = CommandService::getCommandName(command.getName());
    std::map <std::string, std::string> options = command.getOptions();
    if (!CommandService::isCommandValidWithOption(CommandService::getCommandName(command.getName()), command.getOptions()))
    {
        help.commandOptionNotSupported();
        return;
    }

    if (!commandService.isValid(commandName))
    {
        help.commandNotFound();
        return;
    }

    // TODO: implement shortcuts
    switch (commandService.list().getCommandId(commandName))
    {
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
        case CommandList::PENDING:
            status(StatusService::PENDING);
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

void CLIController::show()
{
    if (commandService.hasSubCommand(command) && commandService.getSubCommand(command).getName() == "all")
    {

        ioService.br();
        ioService.info("Showing all lists items ");
        ioService.br();

        // fetch all lists
        std::vector <ListEntity> lists = listService.get();
        for (const ListEntity& list : lists)
        {
            std::string listName = list.getName();
            std::string listVariant = "default";
            Show show(ioService, configService, listService, listItemService, cliThemeService, listName, listVariant);

            std::vector <ListItemEntity> listItems = listItemService.load(list.getName()).loadVariant("default").get();
            if (command.hasOption("priority"))
            {
                try {
                    int priority = listItemService.priority().getIdFromName(command.getOption("priority"));
                    listItemService.filterPriorityAbove(listItems, priority);
                } catch (std::invalid_argument &e) {
                    ioService.br();
                    ioService.error(e.what());
                    ioService.br();
                    return;
                }
            }
            if (command.hasOption("status"))
            {
                try {
                    int status = listItemService.status().getIdFromName(command.getOption("status"));
                    listItemService.filterStatus(listItems, {status});
                } catch (std::invalid_argument &e) {
                    ioService.br();
                    ioService.error(e.what());
                    ioService.br();
                    return;
                }
            }
            if (command.hasOption("deadline"))
            {
                try {
                    listItemService.filterDeadlineBefore(listItems, DateHelpers::relativeDateToTimestamp(command.getOption("deadline")));
                    listItemService.filterStatus(listItems, {0, 1, 2});
                } catch (std::invalid_argument &e) {
                    ioService.br();
                    ioService.error(e.what());
                    ioService.br();
                    return;
                }
            }

            try {
                show.printInAllLoop(listItems, false, false);
            } catch (std::exception &e) {
                ioService.br();
                ioService.error(e.what());
                ioService.br();
            }
        }

        return;
    }

    if (command.hasOption("help"))
    {
        help.show();
        return;
    }
    else if (command.hasOption("version"))
    {
        ioService.message(Help::getVersion());
        return;
    }
    else if (command.hasOption("archive"))
    {
        Show show(ioService, configService, listService, listItemService, cliThemeService, currentList, currentListVariant);

        std::vector <ListItemEntity> listItems = listItemService.loadVariant("archive").get();
        if (command.hasOption("priority"))
        {
            try {
                int priority = listItemService.priority().getIdFromName(command.getOption("priority"));
                listItemService.filterPriorityAbove(listItems, priority);
            } catch (std::invalid_argument &e) {
                ioService.br();
                ioService.error(e.what());
                ioService.br();
                return;
            }
        }
        if (command.hasOption("status"))
        {
            try {
                int status = listItemService.status().getIdFromName(command.getOption("status"));
                listItemService.filterStatus(listItems, {status});
            } catch (std::invalid_argument &e) {
                ioService.br();
                ioService.error(e.what());
                ioService.br();
                return;
            }
        }
        if (command.hasOption("deadline"))
        {
            try {
                listItemService.filterDeadlineBefore(listItems, DateHelpers::relativeDateToTimestamp(command.getOption("deadline")));
                listItemService.filterStatus(listItems, {0, 1, 2});
            } catch (std::invalid_argument &e) {
                ioService.br();
                ioService.error(e.what());
                ioService.br();
                return;
            }
        }

        try {
            show.print(listItems);
        } catch (std::exception &e) {
            ioService.br();
            ioService.error(e.what());
            ioService.br();
        }
        return;
    }
    else if (command.hasOption("delete"))
    {
        Show show(ioService, configService, listService, listItemService, cliThemeService, currentList, currentListVariant);

        std::vector <ListItemEntity> listItems = listItemService.loadVariant("delete").get();
        if (command.hasOption("priority"))
        {
            try {
                int priority = listItemService.priority().getIdFromName(command.getOption("priority"));
                listItemService.filterPriorityAbove(listItems, priority);
            } catch (std::invalid_argument &e) {
                ioService.br();
                ioService.error(e.what());
                ioService.br();
                return;
            }
        }
        if (command.hasOption("status"))
        {
            try {
                int status = listItemService.status().getIdFromName(command.getOption("status"));
                listItemService.filterStatus(listItems, {status});
            } catch (std::invalid_argument &e) {
                ioService.br();
                ioService.error(e.what());
                ioService.br();
                return;
            }
        }
        if (command.hasOption("deadline"))
        {
            try {
                listItemService.filterDeadlineBefore(listItems, DateHelpers::relativeDateToTimestamp(command.getOption("deadline")));
                listItemService.filterStatus(listItems, {0, 1, 2});
            } catch (std::invalid_argument &e) {
                ioService.br();
                ioService.error(e.what());
                ioService.br();
                return;
            }
        }

        try {
            show.print(listItems);
        } catch (std::exception &e) {
            ioService.br();
            ioService.error(e.what());
            ioService.br();
        }
        return;
    }
    else if (command.hasOption("config"))
    {
        Config config(ioService, command, commandService, configService, listService);
        config.make();
        return;
    }

    Show show(ioService, configService, listService, listItemService, cliThemeService, currentList, currentListVariant);

    std::vector <ListItemEntity> listItems = listItemService.get();
    if (command.hasOption("priority"))
        {
            try {
                int priority = listItemService.priority().getIdFromName(command.getOption("priority"));
                listItemService.filterPriorityAbove(listItems, priority);
            } catch (std::invalid_argument &e) {
                ioService.br();
                ioService.error(e.what());
                ioService.br();
                return;
            }
        }
        if (command.hasOption("status"))
        {
            try {
                int status = listItemService.status().getIdFromName(command.getOption("status"));
                listItemService.filterStatus(listItems, {status});
            } catch (std::invalid_argument &e) {
                ioService.br();
                ioService.error(e.what());
                ioService.br();
                return;
            }
        }
        if (command.hasOption("deadline"))
        {
            try {
                listItemService.filterDeadlineBefore(listItems, DateHelpers::relativeDateToTimestamp(command.getOption("deadline")));
                listItemService.filterStatus(listItems, {0, 1, 2});
            } catch (std::invalid_argument &e) {
                ioService.br();
                ioService.error(e.what());
                ioService.br();
                return;
            }
        }


    try {
        show.print(listItems);
    } catch (std::exception &e) {
        ioService.br();
        ioService.error(e.what());
        ioService.br();
    }
}

void CLIController::listItemActions()
{
    ListItemActions listItemActions(ioService, command, commandService, listItemService);
    listItemActions.make();
    Show show(ioService, configService, listService, listItemService, cliThemeService, currentList, currentListVariant);
    std::vector <ListItemEntity> listItems = listItemService.get();
    try {
        show.print(listItems);
    } catch (std::exception &e) {
        ioService.br();
        ioService.error(e.what());
        ioService.br();
    }
}

void CLIController::find()
{
    Show show(ioService, configService, listService, listItemService, cliThemeService, currentList, currentListVariant);
        std::string stringSearch;
        for (const std::string& argument : command.getArguments())
        {
            stringSearch += argument + " ";
        }
        try {
            std::vector <ListItemEntity> listItems;
            if (command.hasOption("archive"))
            {
                listItems = listItemService.load(configService.getCurrentList(), "archive").search(command.getArguments());
                ioService.br();
                ioService.info("Searching for " + stringSearch + "in archived items...");
                ioService.br();
                show.print(listItems);
            }
            else if (command.hasOption("delete"))
            {
                listItems = listItemService.load(configService.getCurrentList(), "delete").search(command.getArguments());
                ioService.br();
                ioService.info("Searching for " + stringSearch + "in deleted items...");
                ioService.br();
                try {
                    show.print(listItems);
                } catch (std::exception &e) {
                    ioService.br();
                    ioService.error(e.what());
                    ioService.br();
                }
            }
            else
            {
                listItems = listItemService.search(command.getArguments());
                ioService.br();
                ioService.info("Searching for " + stringSearch + "...");
                ioService.br();
                try {
                    show.print(listItems);
                } catch (std::exception &e) {
                    ioService.br();
                    ioService.error(e.what());
                    ioService.br();
                }
            }
        } catch (std::exception &e) {
            ioService.br();
            ioService.info("Searching for " + stringSearch + "...");
            ioService.br();
            ioService.error("No item found.");
        }
}

void CLIController::priority(std::string action)
{
    Priority priority(ioService, command, listItemService);
    if (action == "set")
    {
        priority.set();
    } else if (action == "increase")
    {
        priority.increase();
    } else if (action == "decrease")
    {
        priority.decrease();
    }
    Show show(ioService, configService, listService, listItemService, cliThemeService, currentList, currentListVariant);
    std::vector <ListItemEntity> listItems = listItemService.get();
    try {
        show.print(listItems);
    } catch (std::exception &e) {
        ioService.br();
        ioService.error(e.what());
        ioService.br();
    }
}

void CLIController::reset()
{
    std::string answer = ioService.ask("Are you sure you want to reset item(s) to brand new and not mark them as to-do? (y/n) ");
    if (answer == "y" || answer == "yes")
    {
        Status status(ioService, command, listItemService);
        status.reset();
        Show show(ioService, configService, listService, listItemService, cliThemeService, currentList, currentListVariant);
        std::vector <ListItemEntity> listItems = listItemService.get();
        try {
            show.print(listItems);
        } catch (std::exception &e) {
            ioService.br();
            ioService.error(e.what());
            ioService.br();
        }
    }
    else
    {
        ioService.br();
        ioService.message("Action cancelled.");
        ioService.br();
    }
}

void CLIController::status(int statusNumber)
{
    Status status(ioService, command, listItemService);
    if (statusNumber == -1)
    {
        status.set();
    } else
    {
        status.markAs(statusNumber);
    }
    Show show(ioService, configService, listService, listItemService, cliThemeService, currentList, currentListVariant);
    std::vector <ListItemEntity> listItems = listItemService.get();
    try {
        show.print(listItems);
    } catch (std::exception &e) {
        ioService.br();
        ioService.error(e.what());
        ioService.br();
    }
}

void CLIController::remove()
{
    Remove remove(ioService, command, listItemService);
    remove.remove();
    Show show(ioService, configService, listService, listItemService, cliThemeService, currentList, currentListVariant);
    std::vector <ListItemEntity> listItems = listItemService.get();
    try {
        show.print(listItems);
    } catch (std::exception &e) {
        ioService.br();
        ioService.error(e.what());
        ioService.br();
    }
}

void CLIController::archive()
{
    Remove remove(ioService, command, listItemService);
    remove.archive();
    Show show(ioService, configService, listService, listItemService, cliThemeService, currentList, currentListVariant);
    std::vector <ListItemEntity> listItems = listItemService.get();
    try {
        show.print(listItems);
    } catch (std::exception &e) {
        ioService.br();
        ioService.error(e.what());
        ioService.br();
    }
}

void CLIController::restore()
{
    Remove remove(ioService, command, listItemService);
    remove.restore();
    Show show(ioService, configService, listService, listItemService, cliThemeService, currentList, currentListVariant);
    std::vector <ListItemEntity> listItems = listItemService.get();
    try {
        show.print(listItems);
    } catch (std::exception &e) {
        ioService.br();
        ioService.error(e.what());
        ioService.br();
    }
}

void CLIController::list()
{
    ListActions list(ioService, command, commandService, listService, listItemService, fileStorageService, configService, cliThemeService);
    list.make();
}

void CLIController::use()
{
    ListActions list(ioService, command, commandService, listService, listItemService, fileStorageService, configService, cliThemeService);
    list.use();
    currentList = configService.getCurrentList();
    currentListVariant = "default";
    Show show(ioService, configService, listService, listItemService, cliThemeService, currentList, currentListVariant);
    std::vector <ListItemEntity> listItems = listItemService.load(configService.getCurrentList()).get();
    try {
        show.print(listItems);
    } catch (std::exception &e) {
        ioService.br();
        ioService.error(e.what());
        ioService.br();
    }
}

void CLIController::move()
{
    Move move(ioService, configService, command, commandService, listItemService);
    std::string newListName = move.make();
    if (!newListName.empty())
    {
//            listService.use(newListName);
        std::string listVariant = "default";
        Show show(ioService, configService, listService, listItemService, cliThemeService, newListName, listVariant);
        std::vector <ListItemEntity> listItems = listItemService.load(newListName).get();
        try {
            show.print(listItems);
        } catch (std::exception &e) {
            ioService.br();
            ioService.error(e.what());
            ioService.br();
        }
    }
}

void CLIController::empty()
{
    ioService.br();
    std::string message = "Are you sure you want to empty ";
    message += StringHelpers::colorize(configService.getCurrentList(), RED);
    message += " list? (yes/no) ";
    std::string answer = ioService.ask(message);
    if (answer == "yes") {
        listItemService.archiveAll();
        ioService.br();
        ioService.success("List cleared.");
        ioService.br();
    } else {
        ioService.br();
        ioService.message("Action cancelled.");
        ioService.br();
    }
}

void CLIController::clean()
{
    ioService.br();
    std::string message = "Are you sure you want to archive all completed & cancelled from ";
    message += StringHelpers::colorize(configService.getCurrentList(), RED);
    message += " list? (yes/no) ";
    std::string answer = ioService.ask(message);
    if (answer == "yes")
    {
        listItemService.archiveFinishedItems();
        ioService.br();
        ioService.success("List cleaned.");
        ioService.br();
        Show show(ioService, configService, listService, listItemService, cliThemeService, currentList, currentListVariant);
        std::vector <ListItemEntity> listItems = listItemService.get();
        try {
            show.print(listItems);
        } catch (std::exception &e) {
            ioService.br();
            ioService.error(e.what());
            ioService.br();
        }
    }
    else
    {
        ioService.br();
        ioService.message("Action cancelled.");
        ioService.br();
    }
}

void CLIController::stats()
{
    Stats stats(ioService, configService, command, listItemService, cliThemeService);
    stats.print();
}
