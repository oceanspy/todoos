#include "Priority.h"

Priority::Priority(IOService& ioService, Command& command, ListItemService& listItemService)
  : ioService(ioService)
  , command(command)
  , listItemService(listItemService)
{
}

void
Priority::increase(ListName& listName)
{
    if (command.getArguments().empty()) {
        ioService.br();
        ioService.error("Please provide the ID of the element to modify.");
        ioService.br();
        return;
    }

    std::vector<std::string> arguments = command.getArguments();

    int times = 1;
    // if first argument is int, between 1 and 4 and only 1 char long
    // We do the actions multiple times
    if (arguments.at(0).length() == 1 && std::isdigit(arguments.at(0).at(0)) && std::stoi(arguments.at(0)) >= 1 &&
        std::stoi(arguments.at(0)) <= 4) {
        times = std::stoi(arguments.at(0));
        arguments.erase(arguments.begin());
    }

    std::vector<std::string> ids = arguments;

    // clean the double ids
    ids.erase(unique(ids.begin(), ids.end()), ids.end());

    ioService.br();
    for (const auto& id : ids) {
        for (int i = 0; i < times; i++) {
            try {
                listItemService.increasePriority(id, listName);
                ioService.success("Priority of: " + id + " correctly increased.");
            } catch (std::invalid_argument& e) {
                ioService.error("Priority of item with id: " + id + " couldn't be increased.");
                continue;
            }
        }
    }
    ioService.br();
}

void
Priority::decrease(ListName& listName)
{
    if (command.getArguments().empty()) {
        ioService.br();
        ioService.error("Please provide the ID of the element to modify.");
        ioService.br();
        return;
    }

    std::vector<std::string> arguments = command.getArguments();

    int times = 1;
    // if first argument is int, between 1 and 4 and only 1 char long
    // We do the actions multiple times
    if (arguments.at(0).length() == 1 && std::isdigit(arguments.at(0).at(0)) && std::stoi(arguments.at(0)) >= 1 &&
        std::stoi(arguments.at(0)) <= 4) {
        times = std::stoi(arguments.at(0));
        arguments.erase(arguments.begin());
    }

    std::vector<std::string> ids = arguments;

    // clean the double ids
    ids.erase(unique(ids.begin(), ids.end()), ids.end());

    ioService.br();
    for (const auto& id : ids) {
        for (int i = 0; i < times; i++) {
            try {
                listItemService.decreasePriority(id, listName);
                ioService.success("Priority of: " + id + " correctly decreased.");
            } catch (std::invalid_argument& e) {
                ioService.error("Priority of item with id: " + id + " couldn't be decreased.");
                continue;
            }
        }
    }
    ioService.br();
}

void
Priority::set(ListName& listName)
{
    const std::vector<std::string> arguments = command.getArguments();
    std::vector<std::string> adaptedArguments = command.getArguments();

    if (adaptedArguments.empty()) {
        ioService.br();
        ioService.error("Please provide the ID of the element to modify.");
        ioService.br();
        return;
    }

    if (adaptedArguments.size() < 2) {
        ioService.br();
        ioService.error("Please provide the new priority and the ID(s) of the element(s) to modify.");
        ioService.br();
        return;
    }
    std::string* priority;

    if (!listItemService.priority().isNameValid(adaptedArguments.at(0))) {
        ioService.br();
        ioService.error("Please provide a valid priority.");
        ioService.br();
        return;
    }

    priority = new std::string(adaptedArguments.at(0));
    adaptedArguments.erase(adaptedArguments.begin());

    std::vector<std::string> ids = adaptedArguments;

    // clean the double ids
    ids.erase(unique(ids.begin(), ids.end()), ids.end());

    ioService.br();
    for (const auto& id : ids) {
        try {
            listItemService.setPriority(id, listName, priority);
            ioService.success("Priority of: " + id + " correctly updated.");
        } catch (std::invalid_argument& e) {
            ioService.error(e.what());
            continue;
        }
    }
    ioService.br();

    free(priority);
}
