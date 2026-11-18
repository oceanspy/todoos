#include "DetachItemAction.h"

DetachItemAction::DetachItemAction(IOService& ioService, ListItemService& listItemService)
  : ioService(ioService)
  , listItemService(listItemService)
{
}

void
DetachItemAction::execute(Command& command, ListName& listName)
{
    if (command.getArguments().empty()) {
        ioService.br();
        ioService.error("Please provide the ID(s) of the child or parent element(s) to detach.");
        ioService.br();
        return;
    }

    std::vector<std::string> ids = command.getArguments();
    ids.erase(unique(ids.begin(), ids.end()), ids.end());

    ioService.br();
    for (const auto& id : ids) {
        try {
            ListItemEntity listItem = listItemService.find(id, listName);
            if (listItem.hasParent()) {
                listItemService.detach(id, listName);
                ioService.success("Item " + id + " detached from " + *listItem.getParentId() + ".");
                continue;
            }

            const size_t childCount = listItemService.getChildren(id, listName).size();
            if (childCount == 0) {
                ioService.error("Item " + id + " is neither a child nor a parent.");
                continue;
            }

            if (!command.hasOption("force")) {
                ioService.error("Item " + id + " has " + std::to_string(childCount) +
                                " child item(s): use --force to detach them all.");
                continue;
            }

            listItemService.detachChildren(id, listName);
            ioService.success("The " + std::to_string(childCount) + " child item(s) of " + id + " were detached.");
        } catch (std::exception& e) {
            ioService.error("Item " + id + " couldn't be detached.");
            ioService.info(e.what());
        }
    }
    ioService.br();
}
