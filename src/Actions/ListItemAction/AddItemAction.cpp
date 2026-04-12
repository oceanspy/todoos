#include "AddItemAction.h"

AddItemAction::AddItemAction(IOService& ioService, CommandService& commandService, ListItemService& listItemService)
  : ioService(ioService)
  , commandService(commandService)
  , listItemService(listItemService)
{
}

void
AddItemAction::execute(Command& command, ListName& listName)
{
    const std::vector<std::string> arguments = command.getArguments();
    std::vector<std::string> itemValueFromArguments = command.getArguments();

    if (arguments.empty()) {
        ioService.br();
        ioService.error("Please provide the value you want to add.");
        ioService.br();
        return;
    }

    std::string priorityValue;
    std::string* priority = nullptr;
    if (command.hasOption("priority") && listItemService.priority().isNameValid(command.getOption("priority"))) {
        priorityValue = command.getOption("priority");
        priority = &priorityValue;
    }

    std::string statusValue;
    std::string* status = nullptr;
    if (command.hasOption("status") && listItemService.status().isNameValid(command.getOption("status"))) {
        statusValue = command.getOption("status");
        status = &statusValue;
    }

    time_t dueAt = 0;
    if (command.hasOption("deadline")) {
        try {
            std::string dateString = command.getOption("deadline");
            dueAt = DateHelpers::relativeDateToTimestamp(dateString);
        } catch (std::exception& e) {
            ioService.br();
            ioService.error("Invalid deadline date.");
            ioService.br();
            return;
        }
    }

    std::string id;
    try {
        id = listItemService.add(
            listName, StringHelpers::vectorToString(itemValueFromArguments), priority, status, dueAt);
    } catch (std::exception& e) {
        ioService.br();
        ioService.error("Item could not be added.");
        ioService.info(e.what());
        ioService.br();
        return;
    }

    ioService.br();
    ioService.success("Item " + id + " correctly added.");
    ioService.br();
    return;
}
