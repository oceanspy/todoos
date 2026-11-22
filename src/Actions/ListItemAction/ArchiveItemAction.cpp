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

    // Children of a listed parent go with it
    ids = listItemService.withoutChildrenOfListedParents(ids, listName);

    ioService.br();
    for (const auto& id : ids) {
        try {
            const size_t childCount = listItemService.getChildren(id, listName).size();
            std::string archivedId = listItemService.archive(id, listName);
            ioService.success("Item with id: " + id + " correctly archived as " + archivedId +
                              (childCount == 0 ? "" : ", with its " + std::to_string(childCount) + " child item(s)") +
                              ".");
        } catch (std::exception& e) {
            ioService.error("Item with id: " + id + " couldn't be archived.");
            ioService.info(e.what());
        }
    }
    ioService.br();
}
