#include "EditStatusAction.h"

EditStatusAction::EditStatusAction(IOService& ioService, ListItemService& listItemService)
  : ioService(ioService)
  , listItemService(listItemService)
{
}

void
EditStatusAction::execute(Command& command, ListName& listName, int statusNumber)
{
    // markAs with specific status number
    if (command.getArguments().empty()) {
        ioService.br();
        ioService.error("Please provide the ID of the element to modify.");
        ioService.br();
        return;
    }

    std::vector<std::string> ids = command.getArguments();
    ids.erase(unique(ids.begin(), ids.end()), ids.end());

    const bool withAttachedItems = command.hasOption("attached");
    const std::string statusName = StringHelpers::colorize(
        StringHelpers::colorize(listItemService.status().getNameFromId(statusNumber), LIGHT_YELLOW), ITALIC);

    ioService.br();
    for (const auto& id : ids) {
        // With --attached, the open children (not completed / cancelled) follow it, even when the item
        // itself is already in the status
        std::vector<std::string> childIds;
        if (withAttachedItems) {
            childIds =
                listItemService.getChildIdsFollowingStatus(id, listName, listItemService.status().find(statusNumber));
        }

        try {
            listItemService.editStatus(id, listName, &statusNumber);
            ioService.success("Status of: " + id + " correctly updated to " + statusName + ".");
        } catch (std::invalid_argument& e) {
            ioService.error("Status of: " + id + " couldn't be updated or is already set.");
        }

        for (const auto& childId : childIds) {
            try {
                listItemService.editStatus(childId, listName, &statusNumber);
                ioService.success("Status of: " + childId + " (child of " + id + ") correctly updated to " +
                                  statusName + ".");
            } catch (std::invalid_argument& e) {
                ioService.error("Status of: " + childId + " (child of " + id + ") couldn't be updated.");
            }
        }
    }
    ioService.br();
}

void
EditStatusAction::executeReset(Command& command, ListName& listName)
{
    if (command.getArguments().empty()) {
        ioService.br();
        ioService.error("Please provide the ID of the element to reset.");
        ioService.br();
        return;
    }

    if (command.getArguments().at(0) == "all") {
        ioService.br();
        std::string answer = ioService.ask("Are you sure you want to reset all items? (y/n) ");
        ioService.br();

        if (answer == "y" || answer == "yes") {
            std::vector<ListItemEntity> listItems = listItemService.get(listName);

            ioService.br();
            for (const ListItemEntity& listItem : listItems) {
                try {
                    listItemService.reset(*listItem.getId(), listName);
                    ioService.success("Item with id: " + *listItem.getId() + " was reset.");
                } catch (std::exception& e) {
                    ioService.error("Item with id: " + *listItem.getId() + " was not found.");
                    continue;
                }
            }
            ioService.br();
        } else {
            ioService.br();
            ioService.info("Action cancelled.");
            ioService.br();
        }
        return;
    }

    std::vector<std::string> ids = command.getArguments();
    ids.erase(unique(ids.begin(), ids.end()), ids.end());

    ioService.br();
    for (const auto& id : ids) {
        try {
            listItemService.reset(id, listName);
            ioService.success("Item with id: " + id + " was reset.");
        } catch (std::invalid_argument& e) {
            ioService.error("Item with id: " + id + " was not found.");
            continue;
        }
    }
    ioService.br();
}
