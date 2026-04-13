#include "RenameListAction.h"

RenameListAction::RenameListAction(IOService& ioService,
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
RenameListAction::execute(Command& command)
{
    std::vector<std::string> arguments = command.getArguments();

    if (arguments.size() < 2) {
        ioService.error("Please provide the name of the list you want to rename and the new name.");
        return;
    }

    std::string oldListName = removeSpaceFromListName(arguments.at(0));
    std::string newListName = removeSpaceFromListName(arguments.at(1));
    std::string goBackList =
        configService.getUsedListNameStr() == oldListName ? newListName : configService.getUsedListNameStr();

    try {
        fileStorageService.moveFileTo(oldListName, newListName);
        listService.edit(oldListName, newListName);
        listService.use(goBackList);
    } catch (std::invalid_argument& e) {
        ioService.br();
        ioService.error("Error while renaming the file");
        ioService.br();
    }

    ioService.br();
    ioService.success("List " + addSpaceToListName(oldListName) + " moved to " + addSpaceToListName(newListName) + ".");
    ioService.br();
}

std::string
RenameListAction::removeSpaceFromListName(std::string listName)
{
    std::replace(listName.begin(), listName.end(), ' ', '_');
    return listName;
}

std::string
RenameListAction::addSpaceToListName(std::string listName)
{
    std::replace(listName.begin(), listName.end(), '_', ' ');
    return listName;
}
