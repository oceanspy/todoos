#include "CommandService.h"

CommandService::CommandService(CommandList& commandList, CommandOption& commandOption)
  : commandList(commandList)
  , commandOption(commandOption)
{
}

bool
CommandService::isValid(std::string& commandName)
{
    return commandList.isValid(commandName);
}

bool
CommandService::hasSubCommand(Command command)
{
    return !command.getArguments().empty();
}

Command
CommandService::getSubCommand(Command command)
{
    std::vector<std::string> arguments = command.getArguments();
    if (arguments.empty()) {
        return { "", {}, command.getOptions() };
    }

    return { arguments[0], std::vector<std::string>(arguments.begin() + 1, arguments.end()), command.getOptions() };
}

bool
CommandService::isCommand(Command& command, const std::string& commandName)
{
    return CommandShortcut::isCommand(command, commandName);
}

bool
CommandService::isBeginningOfCommand(Command command)
{
    return commandList.isBeginningOfCommand(command.getName());
}

bool
CommandService::isCommandValidWithOption(std::string commandName, std::map<std::string, std::string> options)
{
    return CommandList::isCommandValidWithOptions(commandName, options);
}

std::vector<std::string>
CommandService::getBaseCommandList()
{
    return commandList.getMainCommandNames();
}

std::string
CommandService::getMainCommandListAsString()
{
    std::vector<std::string> baseCommandList = commandList.getMainCommandNames();
    std::string baseCommandsString;
    int i = 0;
    for (const std::string& command : baseCommandList) {
        baseCommandsString += command;
        if (i < baseCommandList.size() - 1) {
            baseCommandsString += " ";
        }
        i++;
    }
    return baseCommandsString;
}

CommandList&
CommandService::list()
{
    return commandList;
}

CommandOption&
CommandService::options()
{
    return commandOption;
}

std::string
CommandService::getCommandName(const std::string& shortcut)
{
    return CommandShortcut::getCommandName(shortcut);
}
