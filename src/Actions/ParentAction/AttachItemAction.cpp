#include "AttachItemAction.h"

AttachItemAction::AttachItemAction(IOService& ioService, ListItemService& listItemService)
  : ioService(ioService)
  , listItemService(listItemService)
{
}

void
AttachItemAction::execute(Command& command, ListName& listName)
{
    if (command.getArguments().empty()) {
        ioService.br();
        ioService.error("Please provide the ID(s) of the element(s) to attach.");
        ioService.br();
        return;
    }

    const std::string parentId = command.getOption("to");
    if (parentId.empty()) {
        ioService.br();
        ioService.error("Please provide the parent item with --to {id}.");
        ioService.br();
        return;
    }

    std::vector<std::string> ids = command.getArguments();
    ids.erase(unique(ids.begin(), ids.end()), ids.end());

    ioService.br();
    for (const auto& id : ids) {
        try {
            const std::string oldParentId = *listItemService.find(id, listName).getParentId();
            listItemService.attach(id, parentId, listName, command.hasOption("force"));
            ioService.success("Item " + id +
                              (oldParentId.empty() ? " attached to " : " moved from " + oldParentId + " to ") +
                              parentId + ".");
        } catch (std::exception& e) {
            ioService.error("Item " + id + " couldn't be attached to " + parentId + ".");
            ioService.info(e.what());
        }
    }
    ioService.br();
}
