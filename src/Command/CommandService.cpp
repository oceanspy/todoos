#include "CommandService.h"

CommandService::CommandService(CommandRegistry& commandRegistry, CommandOption& commandOption)
  : commandRegistry(commandRegistry)
  , commandOption(commandOption)
{
}

bool
CommandService::isValid(Command& command)
{
    return commandRegistry.isValid(CommandService::getCommandName(command.getName()));
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
    return commandRegistry.isBeginningOfCommand(command.getName());
}

bool
CommandService::isCommandValidWithOption(Command& command)
{
    return CommandRegistry::isCommandValidWithOptions(command);
}

std::vector<std::string>
CommandService::getBaseCommandList()
{
    return commandRegistry.getMainCommandNames();
}

std::string
CommandService::getMainCommandListAsString()
{
    std::vector<std::string> baseCommandList = commandRegistry.getMainCommandNames();
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

CommandRegistry&
CommandService::list()
{
    return commandRegistry;
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
