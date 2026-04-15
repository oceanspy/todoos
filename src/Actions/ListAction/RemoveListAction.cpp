#include "RemoveListAction.h"

RemoveListAction::RemoveListAction(IOService& ioService,
                                   ListService& listService,
                                   FileStorageService& fileStorageService,
                                   ConfigService& configService)
  : ioService(ioService)
  , listService(listService)
  , fileStorageService(fileStorageService)
  , configService(configService)
{
}

void
RemoveListAction::execute(Command& command)
{
    std::vector<std::string> arguments = command.getArguments();

    if (arguments.empty()) {
        ioService.error("Please provide a name for the list item.");
        return;
    }

    std::string listName = removeSpaceFromListName(arguments.at(0));
    std::string goBackList = configService.getUsedListNameStr() == listName ? configService.getDefaultList()
                                                                            : configService.getUsedListNameStr();

    try {
        listService.remove(listName);
        if (command.hasOption("force")) {
            fileStorageService.removeListFile(listName);
        } else {
            fileStorageService.moveFileTo(listName, "._backup_del_" + listName);
        }
        listService.use(goBackList);
    } catch (std::invalid_argument& e) {
        ioService.br();
        ioService.error("Error while removing the file -- is list name correct?");
        ioService.br();
        return;
    }

    ioService.br();
    ioService.success("List " + listName + " removed.");
    ioService.br();
}

std::string
RemoveListAction::removeSpaceFromListName(std::string listName)
{
    std::replace(listName.begin(), listName.end(), ' ', '_');
    return listName;
}
