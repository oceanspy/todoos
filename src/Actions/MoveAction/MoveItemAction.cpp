#include "MoveItemAction.h"

MoveItemAction::MoveItemAction(IOService& ioService, ListService& listService, ListItemService& listItemService)
  : ioService(ioService)
  , listService(listService)
  , listItemService(listItemService)
{
}

std::string
MoveItemAction::execute(Command& command, ListName& listName)
{
    const std::vector<std::string> arguments = command.getArguments();

    if (arguments.empty()) {
        ioService.br();
        ioService.error("Please provide the ID(s) of the element(s) to move.");
        ioService.br();
        return "";
    }

    std::string newListStr = command.getOption("to");
    if (newListStr.empty()) {
        ioService.br();
        ioService.error("Please provide the target list with --to {list}.");
        ioService.br();
        return "";
    }

    std::vector<std::string> ids = arguments;
    ids.erase(unique(ids.begin(), ids.end()), ids.end());

    ListName newListName = listService.createListName(newListStr);

    // Children of a listed parent follow it: no need to move them on their own
    ids = listItemService.withoutChildrenOfListedParents(ids, listName);

    if (listName.getName() == newListName.getName()) {
        ioService.br();
        ioService.error("Item is already in list " + newListName.getName() + ".");
        ioService.br();
        return "";
    }

    ioService.br();
    for (const auto& id : ids) {
        try {
            ListItemEntity listItemEntity(listName);
            try {
                listItemEntity = listItemService.find(id, newListName);
            } catch (std::exception& e) {
                //
            }

            if (!(*listItemEntity.getId()).empty() && !command.hasOption("force")) {
                std::string answer = ioService.ask("Item " + id + " already exists in list " + newListName.getName() +
                                                   ". Do you want to overwrite it? (y/n) ");
                if (answer != "y" && answer != "yes") {
                    ioService.br();
                    ioService.info("Item " + id + " was not moved from list " + listName.getName() + " to list " +
                                   newListName.getName() + ".");
                    ioService.br();
                    return "";
                }
            }

            auto movedChildren = listItemService.move(id, listName, newListName);

            ioService.success("Item " + id + " moved from " + listName.getName() + " to list " + newListName.getName() +
                              ".");
            for (const auto& [childId, newChildId] : movedChildren) {
                ioService.info("  child " + childId + " moved with it" +
                               (childId != newChildId ? " (now " + newChildId + ")" : "") + ".");
            }
        } catch (std::exception& e) {
            ioService.error("Item " + id + " could not be moved from list " + listName.getName() + " to " +
                            newListName.getName() + ".");
            ioService.info(e.what());
            return "";
        }
    }
    ioService.br();

    return newListStr;
}
