#include <stdexcept>
#include <algorithm>
#include "CommandList.h"

CommandList::CommandList()
{
    make();
}

/*
 *
 * For a new command, add:
 * the command to the struct in CommandList.h
 * the command id to the commandIds vector
 * and add the command to the commands map with its name
 * => Edit the controllers
 *
 */

void CommandList::make()
{
    commandIds = {
            SHOW,
            ADD,
            EDIT,
            APPEND,
            PREPEND,
            FIND,
            PRIORITY,
            INCREASE,
            DECREASE,
            STATUS,
            TO_DO,
            START,
            PAUSE,
            REVIEW,
            PENDING,
            COMPLETE,
            CANCEL,
            REMOVE,
            ARCHIVE,
            RESTORE,
            MOVE,
            COPY,
            EMPTY,
            CLEAN,
            DUPLICATE,
            DEADLINE,
            LIST,
            USE,
            STATS,
            RESET,
            COMMANDS
    };

    // loop to commandIds to get the command name
    for (int commandId : commandIds) {
        commands[commandId] = CommandEntity();
        commands[commandId].id = commandId;
    }

    for (const auto& [commandId, commandEntity] : commands) {
        switch (commandId) {
            case SHOW:
                commands[commandId].name = "show";
                break;
            case ADD:
                commands[commandId].name = "add";
                break;
            case EDIT:
                commands[commandId].name = "edit";
                break;
            case APPEND:
                commands[commandId].name = "append";
                break;
            case PREPEND:
                commands[commandId].name = "prepend";
                break;
            case FIND:
                commands[commandId].name = "find";
                break;
            case PRIORITY:
                commands[commandId].name = "priority";
                break;
            case INCREASE:
                commands[commandId].name = "increase";
                break;
            case DECREASE:
                commands[commandId].name = "decrease";
                break;
            case STATUS:
                commands[commandId].name = "status";
                break;
            case TO_DO:
                commands[commandId].name = "to-do";
                break;
            case START:
                commands[commandId].name = "start";
                break;
            case PAUSE:
                commands[commandId].name = "pause";
                break;
            case REVIEW:
                commands[commandId].name = "review";
                break;
            case PENDING:
                commands[commandId].name = "pend";
                break;
            case COMPLETE:
                commands[commandId].name = "complete";
                break;
            case CANCEL:
                commands[commandId].name = "cancel";
                break;
            case REMOVE:
                commands[commandId].name = "remove";
                break;
            case ARCHIVE:
                commands[commandId].name = "archive";
                break;
            case RESTORE:
                commands[commandId].name = "restore";
                break;
            case MOVE:
                commands[commandId].name = "move-to";
                break;
            case COPY:
                commands[commandId].name = "copy-to";
                break;
            case EMPTY:
                commands[commandId].name = "empty";
                break;
            case CLEAN:
                commands[commandId].name = "clean";
                break;
            case DUPLICATE:
                commands[commandId].name = "duplicate";
                break;
            case DEADLINE:
                commands[commandId].name = "deadline";
                break;
            case LIST:
                commands[commandId].name = "list";
                break;
            case USE:
                commands[commandId].name = "use";
                break;
            case STATS:
                commands[commandId].name = "stats";
                break;
            case RESET:
                commands[commandId].name = "reset";
                break;
            case COMMANDS:
                commands[commandId].name = "commands";
                commands[commandId].showInAutocomplete = false;
                break;
            default:
                break;
        }
    }
}

bool CommandList::isValid(const std::string &commandNameToEvaluate)
{
    return std::ranges::any_of(commands, [&commandNameToEvaluate](const auto& command) {
        return command.second.name == commandNameToEvaluate;
    });
}

bool CommandList::isBeginningOfCommand(const std::string &partialCommandNameToEvaluate)
{
    return std::ranges::any_of(commands, [&partialCommandNameToEvaluate](const auto& command) {
        return command.second.name.compare(0, partialCommandNameToEvaluate.size(), partialCommandNameToEvaluate) == 0;
    });
}

int CommandList::getCommandId(const std::string &commandName) {
    auto it = std::find_if(commands.begin(), commands.end(),
    [&commandName](const auto& pair) {
        return pair.second.name == commandName;
    });

    if (it != commands.end()) {
        return it->second.id;
    }

    throw std::invalid_argument("Invalid command.");
}

std::vector<std::string> CommandList::getMainCommandNames(bool showOnlyAutocomplete)
{
    std::vector<std::string> commandNames;
    commandNames.reserve(commands.size());
    for (const auto& [commandId, commandEntity] : commands) {
        if (showOnlyAutocomplete && !commandEntity.showInAutocomplete) {
            continue;
        }
        commandNames.push_back(commandEntity.name);
    }

    return commandNames;
}

bool CommandList::isCommandValidWithOptions(const std::string commandName, const std::map<std::string, std::string> options)
{
    if (options.empty() || commandName == "show") {
        return true;
    }

    return std::ranges::any_of(options, [&commandName](const auto& option) {
        if (option.first == "list")
        {
            return true;
        }
        else if (option.first == "priority")
        {
            if (commandName == "show" ||
                commandName == "add" ||
                commandName == "edit" ||
                commandName == "append" ||
                commandName == "prepend"
            ) {
                return true;
            }
        }
        else if (option.first == "status")
        {
            if (commandName == "show" ||
                commandName == "add" ||
                commandName == "edit" ||
                commandName == "append" ||
                commandName == "prepend"
            ) {
                return true;
            }
        }
        else if (option.first == "deadline")
        {
            if (commandName == "show" ||
                commandName == "add" ||
                commandName == "edit" ||
                commandName == "append" ||
                commandName == "prepend"
            ) {
                return true;
            }
        }
        else if (option.first == "archive")
        {
            if (commandName == "show" ||
                commandName == "find"
            ) {
                return true;
            }
        }
        else if (option.first == "delete")
        {
            if (commandName == "show" ||
                commandName == "find"
            ) {
                return true;
            }
        }
        else if (option.first == "force")
        {
            if (commandName == "remove" ||
                commandName == "move-to" ||
                commandName == "copy-to"
            ) {
                return true;
            }
        }
        else if (option.first == "config")
        {
            // only show
            // TODO: implement edit config
        }

        return false;
    });
}

