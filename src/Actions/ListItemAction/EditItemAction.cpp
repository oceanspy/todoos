#include "EditItemAction.h"

EditItemAction::EditItemAction(IOService& ioService, CommandService& commandService, ListItemService& listItemService)
  : ioService(ioService)
  , commandService(commandService)
  , listItemService(listItemService)
{
}

void
EditItemAction::execute(Command& command, ListName& listName)
{
    const std::vector<std::string> arguments = command.getArguments();
    std::vector<std::string> itemValueFromArguments = command.getArguments();

    if (arguments.empty()) {
        ioService.br();
        ioService.error("Please provide the ID and value for the list item.");
        ioService.br();
        return;
    }

    // Get the ID of the list item
    std::string id = itemValueFromArguments.at(0);

    if (!StringHelpers::isAlnum(id)) {
        ioService.br();
        ioService.error("ID is not valid.");
        ioService.br();
        return;
    }

    itemValueFromArguments.erase(itemValueFromArguments.begin());

    try {
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
                listItemService.editDeadline(id, listName, dueAt);
            } catch (std::exception& e) {
                ioService.br();
                ioService.error("Invalid deadline date.");
                ioService.br();
                return;
            }
        }

        std::string value = StringHelpers::vectorToString(itemValueFromArguments);
        if (priority != nullptr) {
            listItemService.setPriority(id, listName, priority);
            ioService.br();
            ioService.success("Priority of " + id + " updated.");
            ioService.br();
            return;
        } else if (status != nullptr) {
            listItemService.setStatus(id, listName, status);
            ioService.br();
            ioService.success("Priority of " + id + " updated.");
            ioService.br();
            return;
        }

        listItemService.edit(id, listName, value, priority, status);
    } catch (std::exception& e) {
        ioService.br();
        ioService.error("Item could not be updated.");
        ioService.br();
        return;
    }

    ioService.br();
    ioService.success("Item " + id + " updated.");
    ioService.br();
    return;
}
