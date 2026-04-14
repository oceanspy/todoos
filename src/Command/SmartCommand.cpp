#include "SmartCommand.h"
#include "CommandService.h"

SmartCommand::SmartCommand() {}

Command
SmartCommand::apply(Command& command)
{
    std::vector<std::string> arguments = command.getArguments();

    // If command is autocomplete, let's apply the smart filter
    // and treat it again
    if (CommandService::isCommand(command, "commands")) {
        if (arguments.empty()) {
            return command;
        }
        std::string name = arguments.at(0);
        arguments.erase(arguments.begin());
        Command autocompleteCommand = Command::create(name, arguments, command.getOptions());
        autocompleteCommand = apply(autocompleteCommand);

        std::vector<std::string> newArguments = autocompleteCommand.getArguments();
        newArguments.insert(newArguments.begin(), autocompleteCommand.getName());
        Command adaptatedCommand = Command::create("commands", newArguments, command.getOptions());
        return adaptatedCommand;
    }

    // "create <args> → "add <args>
    if (command.getName() == "create") {

        // "create list <args> → "add list <args>
        if (!command.getArguments().empty() && command.getArguments().at(0) == "list") {
            arguments.erase(arguments.begin());
            arguments.insert(arguments.begin(), "add");
            Command adaptatedCommand = Command::create("list", arguments, command.getOptions());
            return adaptatedCommand;
        }

        Command adaptatedCommand = Command::create("add", command.getArguments(), command.getOptions());
        return adaptatedCommand;
    } else if (command.getName() == "upper" || command.getName() == "up") {
        Command adaptatedCommand = Command::create("increase", command.getArguments(), command.getOptions());
        return adaptatedCommand;
    } else if (command.getName() == "lower" || command.getName() == "low") {
        Command adaptatedCommand = Command::create("decrease", command.getArguments(), command.getOptions());
        return adaptatedCommand;
    }

    // "add list <args>" → "list add <args>"
    // Allows shorthand like "add list foo" instead of "list add foo"
    if (CommandService::isCommand(command, "add")) {
        if (command.getArguments().empty()) {
            return command;
        }

        if (command.getArguments().at(0) == "list") {
            // remove first argument
            arguments.erase(arguments.begin());
            // add new command to first argument
            arguments.insert(arguments.begin(), "add");
            Command adaptatedCommand = Command::create("list", arguments, command.getOptions());
            return adaptatedCommand;
        }

        // "remove list <args>" → "list remove <args>"
        // Allows shorthand like "remove list foo" instead of "list remove foo"
    } else if (CommandService::isCommand(command, "remove")) {
        if (command.getArguments().empty()) {
            return command;
        }

        if (command.getArguments().at(0) == "list") {
            // remove first argument
            arguments.erase(arguments.begin());
            // add new command to first argument
            arguments.insert(arguments.begin(), "remove");
            Command adaptatedCommand = Command::create("list", arguments, command.getOptions());
            return adaptatedCommand;
        }

        // "current" → "list current"
        // Shorthand to show the current list without typing the full "list current"
    } else if (CommandService::isCommand(command, "current")) {
        return Command::create("list", { "current" }, command.getOptions());

        // "all" → "show all"
        // Shorthand to display all todos without typing "show all"
    } else if (CommandService::isCommand(command, "all")) {
        if (command.getArguments().empty()) {
            // add new command to first argument
            arguments.insert(arguments.begin(), "all");
            Command adaptatedCommand = Command::create("show", arguments, command.getOptions());
            return adaptatedCommand;
        }
    }

    return command;
}
