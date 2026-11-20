#include "SetStatusAction.h"

SetStatusAction::SetStatusAction(IOService& ioService, ListItemService& listItemService)
  : ioService(ioService)
  , listItemService(listItemService)
{
}

void
SetStatusAction::execute(Command& command, ListName& listName)
{
    // Parse status from command arguments
    std::vector<std::string> adaptedArguments = command.getArguments();

    if (adaptedArguments.empty()) {
        ioService.br();
        ioService.error("Please provide the ID of the element to modify.");
        ioService.br();
        return;
    }

    if (adaptedArguments.size() < 2) {
        ioService.br();
        ioService.error("Please provide the new status and the ID(s) of the element(s) to modify.");
        ioService.br();
        return;
    }
    if (!listItemService.status().isNameValid(adaptedArguments.at(0))) {
        ioService.br();
        ioService.error("Please provide a valid status.");
        ioService.br();
        return;
    }

    std::string status = adaptedArguments.at(0);
    adaptedArguments.erase(adaptedArguments.begin());

    std::vector<std::string> ids = adaptedArguments;
    ids.erase(unique(ids.begin(), ids.end()), ids.end());

    const bool withAttachedItems = command.hasOption("attached");

    ioService.br();
    for (const auto& id : ids) {
        // With --attached, the open children (not completed / cancelled) follow it, even when the item
        // itself is already in the status
        std::vector<std::string> childIds;
        if (withAttachedItems) {
            childIds = listItemService.getChildIdsFollowingStatus(
                id, listName, listItemService.status().getStatusFromName(status));
        }

        try {
            listItemService.setStatus(id, listName, &status);
            ioService.success("Status of: " + id + " correctly updated.");
        } catch (std::invalid_argument& e) {
            ioService.error(e.what());
        }

        for (const auto& childId : childIds) {
            try {
                listItemService.setStatus(childId, listName, &status);
                ioService.success("Status of: " + childId + " (child of " + id + ") correctly updated.");
            } catch (std::invalid_argument& e) {
                ioService.error(e.what());
            }
        }
    }
    ioService.br();
}
