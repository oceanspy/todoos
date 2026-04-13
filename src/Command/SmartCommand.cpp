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

    // "create <args> → "add <args>
    if (command.getName() == "create") {

        // "create list <args> → "add list <args>
        if (command.getArguments().at(0) == "list") {
            std::vector<std::string> arguments = command.getArguments();
            // remove first argument
            arguments.erase(arguments.begin());
            // add new command to first argument
            arguments.insert(arguments.begin(), "add");
            Command adaptatedCommand = Command("list", arguments, command.getOptions());
            return adaptatedCommand;
        }

        Command adaptatedCommand = Command("add", command.getArguments(), command.getOptions());
        return adaptatedCommand;
    }

    // "add list <args>" → "list add <args>"
    // Allows shorthand like "add list foo" instead of "list add foo"
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

        // "remove list <args>" → "list remove <args>"
        // Allows shorthand like "remove list foo" instead of "list remove foo"
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

        // "current" → "list current"
        // Shorthand to show the current list without typing the full "list current"
    } else if (CommandService::isCommand(command, "current")) {
        return Command("list", { "current" }, command.getOptions());

        // "all" → "show all"
        // Shorthand to display all todos without typing "show all"
    } else if (CommandService::isCommand(command, "all")) {
        if (command.getArguments().empty()) {
            std::vector<std::string> arguments = command.getArguments();
            // add new command to first argument
            arguments.insert(arguments.begin(), "all");
            Command adaptatedCommand = Command("show", arguments, command.getOptions());
            return adaptatedCommand;
        }
    }

    return command;
}
