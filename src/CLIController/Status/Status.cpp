#include "Status.h"

Status::Status(IOService& ioService, Command& command, ListItemService& listItemService)
    : ioService(ioService), command(command), listItemService(listItemService)
{

}

void Status::markAs(int status)
{
    if (command.getArguments().empty())
    {
        ioService.br();
        ioService.error("Please provide the ID of the element to modify.");
        ioService.br();
        return;
    }

    std::vector <std::string> ids = command.getArguments();

    // clean the double ids
    ids.erase( unique( ids.begin(), ids.end() ), ids.end() );

    ioService.br();
    for (const auto &id : ids)
    {
        try {
            listItemService.editStatus(id, &status);
            ioService.success("Status of: " + id + " correctly updated to " + StringHelpers::colorize(StringHelpers::colorize(
                    listItemService.status().getNameFromId(status), LIGHT_YELLOW), ITALIC) + ".");
        } catch (std::invalid_argument &e) {
            ioService.error("Status of: " + id + " couldn't be updated or is already set.");
            continue;
        }
    }
    ioService.br();
}

void Status::reset() {
    if (command.getArguments().empty())
    {
        ioService.br();
        ioService.error("Please provide the ID of the element to reset.");
        ioService.br();
        return;
    }

    if (command.getArguments().at(0) == "all")
    {
        ioService.br();
        std::string answer = ioService.ask("Are you sure you want to reset all items? (y/n) ");
        ioService.br();

        if (answer == "y" || answer == "yes")
        {
            std::vector <ListItemEntity> listItems = listItemService.get();

            ioService.br();
            for (const ListItemEntity &listItem : listItems)
            {
                try {
                    listItemService.reset(listItem.getId());
                    ioService.success("Item with id: " + listItem.getId() + " was reset.");
                } catch (std::exception &e) {
                    ioService.error("Item with id: " + listItem.getId() + " was not found.");
                    continue;
                }
            }
            ioService.br();
        }
        else
        {
            ioService.br();
            ioService.info("Action cancelled.");
            ioService.br();
        }
        return;
    }

    std::vector <std::string> ids = command.getArguments();

    // clean the double ids
    ids.erase( unique( ids.begin(), ids.end() ), ids.end() );

    ioService.br();
    for (const auto &id : ids)
    {
        try {
            listItemService.reset(id);
            ioService.success("Item with id: " + id + " was reset.");
        } catch (std::invalid_argument &e) {
            ioService.error("Item with id: " + id + " was not found.");
            continue;
        }
    }
    ioService.br();
}

void Status::set()
{
    const std::vector <std::string> arguments = command.getArguments();
    std::vector <std::string> adaptedArguments = command.getArguments();

    if (adaptedArguments.empty())
    {
        ioService.br();
        ioService.error("Please provide the ID of the element to modify.");
        ioService.br();
        return;
    }

    if (adaptedArguments.size() < 2)
    {
        ioService.br();
        ioService.error("Please provide the new status and the ID(s) of the element(s) to modify.");
        ioService.br();
        return;
    }
    std::string* status;

    if (!listItemService.status().isNameValid(adaptedArguments.at(0)))
    {
        ioService.br();
        ioService.error("Please provide a valid status.");
        ioService.br();
        return;
    }

    status = new std::string(adaptedArguments.at(0));
    adaptedArguments.erase(adaptedArguments.begin());

    std::vector <std::string> ids = adaptedArguments;

    // clean the double ids
    ids.erase( unique( ids.begin(), ids.end() ), ids.end() );

    ioService.br();
    for (const auto &id : ids)
    {
        try {
            listItemService.setStatus(id, status);
            ioService.success("Status of: " + id + " correctly updated.");
        } catch (std::invalid_argument &e) {
            ioService.error(e.what());
            continue;
        }
    }
    ioService.br();

    free(status);
}
