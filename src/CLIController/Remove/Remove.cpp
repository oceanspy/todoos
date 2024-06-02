#include "Remove.h"

Remove::Remove(IOService& ioService, Command& command, ListItemService& listItemService)
    : ioService(ioService), command(command), listItemService(listItemService)
{

}

void Remove::remove()
{
    if (command.getArguments().empty())
    {
        ioService.br();
        ioService.error("Please provide the ID of the element to remove.");
        ioService.br();
        return;
    }

    std::vector <std::string> ids = command.getArguments();

    // clean the double ids
    ids.erase( unique( ids.begin(), ids.end() ), ids.end() );

    ioService.br();
    for (const auto &id : ids)
    {
        try
        {
            if (command.hasOption("force"))
            {
                listItemService.remove(id);
                ioService.success("Item with id: " + id + " correctly removed.");
                continue;
            }

            listItemService.softDelete(id);
            ioService.success("Item with id: " + id + " correctly removed.");
        } catch (std::exception &e)
        {
            ioService.error("Item with id: " + id + " couldn't be deleted.");
        }
        continue;
    }
    ioService.br();
}

void Remove::restore()
{
    if (command.getArguments().empty())
    {
        ioService.br();
        ioService.error("Please provide the ID of the element to restore.");
        ioService.br();
        return;
    }

    std::vector <std::string> ids = command.getArguments();

    // clean the double ids
    ids.erase( unique( ids.begin(), ids.end() ), ids.end() );

    ioService.br();
    for (const auto &id : ids)
    {
        try
        {
            listItemService.restore(id);
            ioService.success("Item with id: " + id + " correctly restored.");
        } catch (std::exception &e)
        {
            ioService.error("Item with id: " + id + " couldn't be restored.");
            ioService.info(e.what());
        }
    }
    ioService.br();
}

void Remove::archive()
{
    if (command.getArguments().empty())
    {
        ioService.br();
        ioService.error("Please provide the ID of the element to archive.");
        ioService.br();
        return;
    }

    std::vector <std::string> ids = command.getArguments();

    // clean the double ids
    ids.erase( unique( ids.begin(), ids.end() ), ids.end() );

    ioService.br();
    for (const auto &id : ids)
    {
        try
        {
            listItemService.archive(id);
            ioService.success("Item with id: " + id + " correctly archived.");
        } catch (std::exception &e)
        {
            ioService.error("Item with id: " + id + " couldn't be archived.");
        }
    }
    ioService.br();
}
