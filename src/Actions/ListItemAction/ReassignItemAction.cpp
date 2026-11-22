#include "ReassignItemAction.h"

ReassignItemAction::ReassignItemAction(IOService& ioService, ListItemService& listItemService)
  : ioService(ioService)
  , listItemService(listItemService)
{
}

void
ReassignItemAction::execute(Command& command, ListName& listName)
{
    if (command.getArguments().size() != 1) {
        ioService.br();
        ioService.error("Please provide the ID of the element to reassign (one at a time).");
        ioService.br();
        return;
    }

    const std::string newId = command.getOption("to");
    if (newId.empty()) {
        ioService.br();
        ioService.error("Please provide the new id with --to {newId}.");
        ioService.br();
        return;
    }

    const std::string id = command.getArguments().at(0);

    ioService.br();
    try {
        listItemService.reassign(id, newId, listName);
        ioService.success("Item " + id + " is now " + newId + ".");
    } catch (std::exception& e) {
        ioService.error("Item " + id + " couldn't be reassigned.");
        ioService.info(e.what());
    }
    ioService.br();
}
