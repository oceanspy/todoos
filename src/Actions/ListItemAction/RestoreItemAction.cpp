#include "RestoreItemAction.h"

RestoreItemAction::RestoreItemAction(IOService& ioService, ListItemService& listItemService)
  : ioService(ioService)
  , listItemService(listItemService)
{
}

void
RestoreItemAction::execute(Command& command, ListName& listName)
{
    if (command.getArguments().empty()) {
        ioService.br();
        ioService.error("Please provide the ID of the element to restore.");
        ioService.br();
        return;
    }

    std::vector<std::string> ids = command.getArguments();
    ids.erase(unique(ids.begin(), ids.end()), ids.end());

    ioService.br();
    for (const auto& id : ids) {
        try {
            listItemService.restore(id, listName);
            ioService.success("Item with id: " + id + " correctly restored.");
        } catch (std::exception& e) {
            ioService.error("Item with id: " + id + " couldn't be restored.");
            ioService.info(e.what());
        }
    }
    ioService.br();
}
