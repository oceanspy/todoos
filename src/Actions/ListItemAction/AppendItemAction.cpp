#include "AppendItemAction.h"

AppendItemAction::AppendItemAction(IOService& ioService,
                                   CommandService& commandService,
                                   ListItemService& listItemService)
  : ioService(ioService)
  , commandService(commandService)
  , listItemService(listItemService)
{
}

void
AppendItemAction::execute(Command& command, ListName& listName)
{
    const std::vector<std::string> arguments = command.getArguments();
    std::vector<std::string> itemValueFromArguments = command.getArguments();

    if (arguments.size() < 2) {
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
        listItemService.append(id, listName, StringHelpers::vectorToString(itemValueFromArguments));
        if (command.hasOption("priority") && listItemService.priority().isNameValid(command.getOption("priority"))) {
            std::string priority = command.getOption("priority");
            listItemService.setPriority(id, listName, &priority);
        }
        if (command.hasOption("status") && listItemService.status().isNameValid(command.getOption("status"))) {
            std::string status = command.getOption("status");
            listItemService.setStatus(id, listName, &status);
        }
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
