#include "RemoveAction.h"

RemoveAction::RemoveAction(IOService& ioService, ListItemService& listItemService)
  : ioService(ioService)
  , listItemService(listItemService)
{
}

void
RemoveAction::execute(Command& command, ListName& listName, const std::string& action)
{
    if (action == "remove") {
        remove(command, listName);
    } else if (action == "restore") {
        restore(command, listName);
    } else if (action == "archive") {
        archive(command, listName);
    }
}

void
RemoveAction::remove(Command& command, ListName& listName)
{
    if (command.getArguments().empty()) {
        ioService.br();
        ioService.error("Please provide the ID of the element to remove.");
        ioService.br();
        return;
    }

    std::vector<std::string> ids = command.getArguments();

    // clean the double ids
    ids.erase(unique(ids.begin(), ids.end()), ids.end());

    ioService.br();
    for (const auto& id : ids) {
        try {
            if (command.hasOption("force")) {
                listItemService.remove(id, listName);
                ioService.success("Item with id: " + id + " correctly removed.");
                continue;
            }

            listItemService.softDelete(id, listName);
            ioService.success("Item with id: " + id + " correctly removed.");
        } catch (std::exception& e) {
            ioService.error("Item with id: " + id + " couldn't be deleted.");
        }
        continue;
    }
    ioService.br();
}

void
RemoveAction::restore(Command& command, ListName& listName)
{
    if (command.getArguments().empty()) {
        ioService.br();
        ioService.error("Please provide the ID of the element to restore.");
        ioService.br();
        return;
    }

    std::vector<std::string> ids = command.getArguments();

    // clean the double ids
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

void
RemoveAction::archive(Command& command, ListName& listName)
{
    if (command.getArguments().empty()) {
        ioService.br();
        ioService.error("Please provide the ID of the element to archive.");
        ioService.br();
        return;
    }

    std::vector<std::string> ids = command.getArguments();

    // clean the double ids
    ids.erase(unique(ids.begin(), ids.end()), ids.end());

    ioService.br();
    for (const auto& id : ids) {
        try {
            listItemService.archive(id, listName);
            ioService.success("Item with id: " + id + " correctly archived.");
        } catch (std::exception& e) {
            ioService.error("Item with id: " + id + " couldn't be archived.");
        }
    }
    ioService.br();
}
