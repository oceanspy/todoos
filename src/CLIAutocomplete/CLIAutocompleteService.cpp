#include "CLIAutocompleteService.h"

CLIAutocompleteService::CLIAutocompleteService(IOService& ioService,
                                               CommandService& commandService,
                                               Command& command,
                                               ListService& listService,
                                               ListItemService& listItemService)
  : ioService(ioService)
  , commandService(commandService)
  , command(command)
  , listService(listService)
  , listItemService(listItemService)
{
}

bool
CLIAutocompleteService::getCompletion()
{
    if (!CommandService::isCommand(command, "commands")) {
        return false;
    }

    // Autocomplete for options
    if (!command.getOptions().empty()) {
        if (command.hasOption("list") && !listService.isListExist(command.getOption("list"))) {
            try {
                autocompleteOptionList();
            } catch (std::exception& e) {
                return true;
            }
            return true;
        } else if (command.hasOption("priority") &&
                   !listItemService.priority().isNameValid(command.getOption("priority"))) {
            try {
                autocompletePriority(command);
            } catch (std::exception& e) {
                return true;
            }
            return true;
        } else if (command.hasOption("status") && !listItemService.status().isNameValid(command.getOption("status"))) {
            try {
                autocompleteStatus(command);
            } catch (std::exception& e) {
                return true;
            }
            return true;
        } else if (command.hasOption("deadline")) {
            try {
                ioService.print(getDeadline());
            } catch (std::exception& e) {
                return true;
            }
            return true;
        }
    }

    // All base commands if nothing is already given
    if (!commandService.hasSubCommand(command)) {
        try {
            autocompleteBase();
        } catch (std::exception& e) {
            return true;
        }
        return true;
    }

    Command firstSubCommand = commandService.getSubCommand(command);
    std::string firstSubCommandName = CommandService::getCommandName(firstSubCommand.getName());
    if (!commandService.isValid(firstSubCommandName) && !commandService.hasSubCommand(firstSubCommand) &&
        commandService.isBeginningOfCommand(firstSubCommand)) {
        autocompleteBase();
        return true;
    }

    if (CommandService::isCommand(firstSubCommand, "add")) {
        return true;
    } else if (CommandService::isCommand(firstSubCommand, "append") ||
               CommandService::isCommand(firstSubCommand, "prepend")) {
        try {
            autocompleteId(firstSubCommand);
        } catch (std::exception& e) {
            return true;
        }
        return true;
    } else if (CommandService::isCommand(firstSubCommand, "edit")) {
        if (!commandService.hasSubCommand(firstSubCommand)) {
            try {
                autocompleteId(firstSubCommand);
            } catch (std::exception& e) {
                return true;
            }
            return true;
        }

        Command secondSubCommand = commandService.getSubCommand(firstSubCommand);

        if (!isValidListItemId(secondSubCommand.getName())) {
            try {
                autocompleteId(firstSubCommand);
            } catch (std::exception& e) {
                return true;
            }
            return true;
        }

        try {
            ListItemEntity listItemEntity = listItemService.find(secondSubCommand.getName());
            if (!(*listItemEntity.getId()).empty()) {
                std::string value = *listItemEntity.getValue();
                std::string escapedValue = StringHelpers::escapeChar(value, ' ');
                ioService.print("\"" + escapedValue + "\"");
                return true;
            }
        } catch (std::exception& e) {
        }

        return true;
    } else if (CommandService::isCommand(firstSubCommand, "deadline")) {
        std::string deadline = getDeadline();
        if (!commandService.hasSubCommand(firstSubCommand)) {
            ioService.print(deadline);
            return true;
        }

        if (commandService.hasSubCommand(firstSubCommand)) {
            Command secondSubCommand = commandService.getSubCommand(firstSubCommand);
            if (!StringHelpers::containsWord(deadline, secondSubCommand.getName()) &&
                !DateHelpers::isDateValidFromUser(secondSubCommand.getName())) {
                ioService.print(deadline);
                return true;
            }
            autocompleteIdIndefinitely(secondSubCommand);
            return true;
        }
        ioService.print(deadline);

        try {
            autocompleteId(firstSubCommand);
        } catch (std::exception& e) {
            return true;
        }

        return true;
    } else if (CommandService::isCommand(firstSubCommand, "remove") ||
               CommandService::isCommand(firstSubCommand, "archive") ||
               CommandService::isCommand(firstSubCommand, "to-do") ||
               CommandService::isCommand(firstSubCommand, "start") ||
               CommandService::isCommand(firstSubCommand, "pause") ||
               CommandService::isCommand(firstSubCommand, "review") ||
               CommandService::isCommand(firstSubCommand, "pend") ||
               CommandService::isCommand(firstSubCommand, "complete") ||
               CommandService::isCommand(firstSubCommand, "cancel") ||
               CommandService::isCommand(firstSubCommand, "reset") ||
               CommandService::isCommand(firstSubCommand, "increase") ||
               CommandService::isCommand(firstSubCommand, "decrease")) {
        try {
            autocompleteIdIndefinitely(firstSubCommand);
        } catch (std::exception& e) {
            return true;
        }

        return true;
    } else if (CommandService::isCommand(firstSubCommand, "restore")) {
        try {
            autocompleteIdIndefinitely(firstSubCommand, { "archive", "delete" });
        } catch (std::exception& e) {
            return true;
        }

        return true;
    } else if (CommandService::isCommand(firstSubCommand, "priority")) {
        try {
            autocompletePriority(firstSubCommand);
        } catch (std::exception& e) {
            return true;
        }

        return true;
    } else if (CommandService::isCommand(firstSubCommand, "status")) {
        try {
            autocompleteStatus(firstSubCommand);
        } catch (std::exception& e) {
            return true;
        }

        return true;
    } else if (CommandService::isCommand(firstSubCommand, "list")) {
        try {
            autocompleteList(firstSubCommand);
        } catch (std::exception& e) {
            return true;
        }

        return true;
    } else if (CommandService::isCommand(firstSubCommand, "use")) {
        try {
            autocompleteUseList(firstSubCommand);
        } catch (std::exception& e) {
            return true;
        }

        return true;
    } else if (CommandService::isCommand(firstSubCommand, "move-to") ||
               CommandService::isCommand(firstSubCommand, "copy-to") ||
               CommandService::isCommand(firstSubCommand, "duplicate")) {
        try {
            autocompleteMoveList(firstSubCommand);
        } catch (std::exception& e) {
            return true;
        }

        return true;
    }
    return true;
}

void
CLIAutocompleteService::getAllLists(std::string& listString)
{
    std::vector<ListEntity> lists = listService.get();
    int i = 0;
    for (const ListEntity& list : lists) {
        listString += *list.getName();
        if (i < lists.size()) {
            listString += " ";
        }
        i++;
    }
    // remove last space
    listString.pop_back();
}

void
CLIAutocompleteService::autocompleteOptionList()
{
    std::string listString;
    getAllLists(listString);
    std::string temp = command.getLastCommandInput();

    // get last word of the raw command
    std::string lastWord = temp.substr(temp.find_last_of(" ") + 1);
    if (!StringHelpers::containsWord(listString, lastWord) &&
        (!StringHelpers::containsWord(listString, command.getOptions().at("list")))) {
        ioService.print(listString);
    }
}

void
CLIAutocompleteService::autocompleteBase()
{
    ioService.print(commandService.getMainCommandListAsString());
}

void
CLIAutocompleteService::autocompletePriority(const Command& firstSubCommand)
{
    std::string priorityList = listItemService.priority().getNamesAsString();

    if (commandService.hasSubCommand(firstSubCommand)) {
        Command secondSubCommand = commandService.getSubCommand(firstSubCommand);
        if (!StringHelpers::containsWord(priorityList, secondSubCommand.getName())) {
            ioService.print(priorityList);
            return;
        }
        autocompleteIdIndefinitely(secondSubCommand);
        return;
    }
    ioService.print(priorityList);
}

void
CLIAutocompleteService::autocompleteStatus(const Command& firstSubCommand)
{
    std::string statusList = listItemService.status().getCommandNamesAsString();

    if (commandService.hasSubCommand(firstSubCommand)) {
        Command secondSubCommand = commandService.getSubCommand(firstSubCommand);
        if (!StringHelpers::containsWord(statusList, secondSubCommand.getName())) {
            ioService.print(statusList);
            return;
        }
        autocompleteIdIndefinitely(secondSubCommand);
        return;
    }
    ioService.print(statusList);
}

void
CLIAutocompleteService::autocompleteMoveList(const Command& firstSubCommand)
{
    std::string listString;
    getAllLists(listString);
    if (!commandService.hasSubCommand(firstSubCommand)) {
        ioService.print(listString);
        return;
    }

    if (commandService.hasSubCommand(firstSubCommand)) {
        Command secondSubCommand = commandService.getSubCommand(firstSubCommand);
        if (!StringHelpers::containsWord(listString, secondSubCommand.getName())) {
            ioService.print(listString);
            return;
        }
        autocompleteIdIndefinitely(secondSubCommand);
        return;
    }
    ioService.print(listString);
}

void
CLIAutocompleteService::autocompleteList(const Command& firstSubCommand)
{
    std::string listActions = "add rename remove show copy";

    if (!commandService.hasSubCommand(firstSubCommand)) {
        ioService.print(listActions);
        return;
    }

    Command secondSubCommand = commandService.getSubCommand(firstSubCommand);

    if (!StringHelpers::containsWord(listActions, secondSubCommand.getName()) &&
        StringHelpers::isBeginningOfAnyWords(listActions, secondSubCommand.getName())) {
        ioService.print(listActions);
        return;
    }

    Command thirdSubCommand = commandService.getSubCommand(secondSubCommand);
    if (CommandService::isCommand(secondSubCommand, "add")) {
        return;
    } else if (CommandService::isCommand(secondSubCommand, "rename") ||
               CommandService::isCommand(secondSubCommand, "remove") ||
               CommandService::isCommand(secondSubCommand, "copy")) {
        std::string listString;
        getAllLists(listString);
        std::string temp = command.getLastCommandInput();

        // get last word of the raw command
        std::string lastWord = temp.substr(temp.find_last_of(" ") + 1);
        if (!StringHelpers::containsWord(listString, lastWord) &&
            !StringHelpers::containsWord(listString, thirdSubCommand.getName())) {
            ioService.print(listString);
        }
    }
}

void
CLIAutocompleteService::autocompleteUseList(const Command& firstSubCommand)
{
    Command secondSubCommand = commandService.getSubCommand(firstSubCommand);
    std::string listString;
    getAllLists(listString);
    if (!StringHelpers::containsWord(listString, secondSubCommand.getName())) {
        ioService.print(listString);
    }
}

void
CLIAutocompleteService::showListItemId(const std::vector<std::string>& variants)
{
    std::string listItemIds;
    for (const std::string& variant : variants) {
        std::vector<ListItemEntity> listItems = listItemService.loadVariant(variant).get();
        int i = 0;
        for (const ListItemEntity& list : listItems) {
            listItemIds += *list.getId();
            if (i < listItems.size() - 1) {
                listItemIds += " ";
            }
            i++;
        }
    }

    ioService.print(listItemIds);
}

bool
CLIAutocompleteService::isValidListItemId(std::string id)
{
    try {
        ListItemEntity listItemEntity = listItemService.find(id);
        if (!(*listItemEntity.getId()).empty()) {
            return true;
        }
    } catch (std::exception& e) {
    }

    return false;
}

void
CLIAutocompleteService::autocompleteId(const Command& firstSubCommand, const std::vector<std::string>& variants)
{
    if (commandService.hasSubCommand(firstSubCommand)) {
        Command subSubCommand = commandService.getSubCommand(firstSubCommand);
        if (isValidListItemId(subSubCommand.getName())) {
            return;
        }
    }
    showListItemId(variants);
}

void
CLIAutocompleteService::autocompleteIdIndefinitely(const Command& firstSubCommand,
                                                   const std::vector<std::string>& variants)
{
    if (commandService.hasSubCommand(firstSubCommand)) {
        Command subSubCommand = commandService.getSubCommand(firstSubCommand);
        try {
            for (const std::string& variant : variants) {
                ListItemEntity listItemEntity = listItemService.loadVariant(variant).find(subSubCommand.getName());
            }
        } catch (std::exception& e) {
        }
    }
    showListItemId(variants);
}

bool
CLIAutocompleteService::isStartOfCommand(std::string listOfCommandNames, std::string partialCommandName)
{
    std::vector<std::string> commandNames = StringHelpers::split(listOfCommandNames, ' ');
    for (const std::string& commandName : commandNames) {
        if (commandName.compare(0, partialCommandName.size(), partialCommandName) == 0) {
            return true;
        }
    }
    return false;
}

std::string
CLIAutocompleteService::getDeadline()
{
    return "today tomorrow monday tuesday wednesday thursday friday saturday sunday next-week next-month next-year "
           "reset";
}
