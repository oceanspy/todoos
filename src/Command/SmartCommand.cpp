#include "SmartCommand.h"

#include <utility>

SmartCommand::SmartCommand(const Command& command)
  : command(command)
{
}

Command
SmartCommand::apply()
{
    std::string name = command.getName();

    if (CommandService::isCommand(command, "add")) {
        if (command.getArguments().empty()) {
            return command;
        }

        if (command.getArguments().at(0) == "list") {
            std::vector<std::string> arguments = command.getArguments();
            // remove first argument
            arguments.erase(arguments.begin());
            // add new command to first argument
            arguments.insert(arguments.begin(), "add");
            Command adaptatedCommand = Command("list", arguments, command.getOptions());
            return adaptatedCommand;
        }
    } else if (CommandService::isCommand(command, "all")) {
        if (command.getArguments().empty()) {
            std::vector<std::string> arguments = command.getArguments();
            // add new command to first argument
            arguments.insert(arguments.begin(), "all");
            Command adaptatedCommand = Command("show", arguments, command.getOptions());
            return adaptatedCommand;
        }
    } else if (CommandService::isCommand(command, "remove")) {
        if (command.getArguments().empty()) {
            return command;
        }

        if (command.getArguments().at(0) == "list") {
            std::vector<std::string> arguments = command.getArguments();
            // remove first argument
            arguments.erase(arguments.begin());
            // add new command to first argument
            arguments.insert(arguments.begin(), "remove");
            Command adaptatedCommand = Command("list", arguments, command.getOptions());
            return adaptatedCommand;
        }
    } else if (CommandService::isCommand(command, "current")) {
        return Command("list", { "current" }, command.getOptions());
    }

    return command;
}
