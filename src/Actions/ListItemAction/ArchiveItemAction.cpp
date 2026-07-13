#include "ArchiveItemAction.h"

ArchiveItemAction::ArchiveItemAction(IOService& ioService, ListItemService& listItemService)
  : ioService(ioService)
  , listItemService(listItemService)
{
}

void
ArchiveItemAction::execute(Command& command, ListName& listName)
{
    if (command.getArguments().empty()) {
        ioService.br();
        ioService.error("Please provide the ID of the element to archive.");
        ioService.br();
        return;
    }

    std::vector<std::string> ids = command.getArguments();
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
