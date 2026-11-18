#include "RemoveItemAction.h"

RemoveItemAction::RemoveItemAction(IOService& ioService, ListItemService& listItemService, DescriptionRepository& descriptionRepository)
  : ioService(ioService)
  , listItemService(listItemService)
  , descriptionRepository(descriptionRepository)
{
}

void
RemoveItemAction::execute(Command& command, ListName& listName)
{
    if (command.getArguments().empty()) {
        ioService.br();
        ioService.error("Please provide the ID of the element to remove.");
        ioService.br();
        return;
    }

    std::vector<std::string> ids = command.getArguments();
    ids.erase(unique(ids.begin(), ids.end()), ids.end());

    // Children of a listed parent go with it
    ids = listItemService.withoutChildrenOfListedParents(ids, listName);

    const bool permanently = command.hasOption("force-permanently");
    const bool forced = permanently || command.hasOption("force");

    ioService.br();
    for (const auto& id : ids) {
        try {
            const size_t childCount = listItemService.getChildren(id, listName).size();
            if (childCount > 0 && !forced) {
                ioService.error("Item with id: " + id + " has " + std::to_string(childCount) +
                                " child item(s): use -f to remove them together (-F to delete them for good).");
                continue;
            }

            if (permanently) {
                std::vector<std::string> childIds = listItemService.removePermanently(id, listName);
                ioService.success("Item with id: " + id + " definitely removed" +
                                  (childIds.empty() ? "" : ", with its " + std::to_string(childIds.size()) +
                                                               " child item(s)") +
                                  ".");
                continue;
            }

            std::string deletedId = listItemService.softDelete(id, listName);
            ioService.success("Item with id: " + id + " correctly removed (restorable as " + deletedId + ")" +
                              (childCount == 0 ? "" : ", with its " + std::to_string(childCount) + " child item(s)") +
                              ".");
        } catch (std::exception& e) {
            ioService.error("Item with id: " + id + " couldn't be deleted.");
        }
    }
    ioService.br();
}
