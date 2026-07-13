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

    ioService.br();
    for (const auto& id : ids) {
        try {
            if (command.hasOption("force")) {
                listItemService.remove(id, listName);
                descriptionRepository.remove(id, listName);
                ioService.success("Item with id: " + id + " definitely removed.");
                continue;
            }

            listItemService.softDelete(id, listName);
            ioService.success("Item with id: " + id + " correctly removed.");
        } catch (std::exception& e) {
            ioService.error("Item with id: " + id + " couldn't be deleted.");
        }
    }
    ioService.br();
}
