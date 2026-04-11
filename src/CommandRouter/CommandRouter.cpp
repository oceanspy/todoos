#include "CommandRouter.h"
#include "../UseCase/ArchiveUseCase.h"
#include "../UseCase/CleanUseCase.h"
#include "../UseCase/EmptyUseCase.h"
#include "../UseCase/FindUseCase.h"
#include "../UseCase/ListItemActionsUseCase.h"
#include "../UseCase/ListUseCase.h"
#include "../UseCase/MoveUseCase.h"
#include "../UseCase/PriorityUseCase.h"
#include "../UseCase/RemoveUseCase.h"
#include "../UseCase/ResetUseCase.h"
#include "../UseCase/RestoreUseCase.h"
#include "../UseCase/ShowUseCase.h"
#include "../UseCase/StatsUseCase.h"
#include "../UseCase/StatusUseCase.h"
#include "../UseCase/SwitchListUseCase.h"

CommandRouter::CommandRouter(IOService& ioService,
                             Help& help,
                             CommandService& commandService,
                             ConfigService& configService,
                             FileStorageService& fileStorageService,
                             ListService& listService,
                             ListItemService& listItemService,
                             ThemeService& themeService)
  : ioService(ioService)
  , help(help)
  , commandService(commandService)
  , configService(configService)
  , fileStorageService(fileStorageService)
  , listService(listService)
  , listItemService(listItemService)
  , themeService(themeService)
{
}

void
CommandRouter::execute(Command& command)
{
    switch (commandService.list().getCommandId(CommandService::getCommandName(command.getName()))) {
        case CommandList::SHOW:
            ShowUseCase(
                ioService, help, commandService, command, configService, listService, listItemService, themeService)
                .execute();
            return;
        case CommandList::ADD:
        case CommandList::EDIT:
        case CommandList::APPEND:
        case CommandList::PREPEND:
        case CommandList::DEADLINE:
            ListItemActionsUseCase(
                ioService, command, commandService, listItemService, listService, configService, themeService)
                .execute();
            return;
        case CommandList::FIND:
            FindUseCase(ioService, command, configService, listService, listItemService, themeService).execute();
            return;
        case CommandList::PRIORITY:
            PriorityUseCase(ioService, command, listItemService, listService, configService, themeService, "set")
                .execute();
            return;
        case CommandList::INCREASE:
            PriorityUseCase(ioService, command, listItemService, listService, configService, themeService, "increase")
                .execute();
            return;
        case CommandList::DECREASE:
            PriorityUseCase(ioService, command, listItemService, listService, configService, themeService, "decrease")
                .execute();
            return;
        case CommandList::STATUS:
            StatusUseCase(ioService, command, listItemService, listService, configService, themeService, -1).execute();
            return;
        case CommandList::TO_DO:
            StatusUseCase(
                ioService, command, listItemService, listService, configService, themeService, StatusService::TO_DO)
                .execute();
            return;
        case CommandList::START:
            StatusUseCase(
                ioService, command, listItemService, listService, configService, themeService, StatusService::STARTED)
                .execute();
            return;
        case CommandList::REVIEW:
            StatusUseCase(
                ioService, command, listItemService, listService, configService, themeService, StatusService::REVIEWING)
                .execute();
            return;
        case CommandList::PAUSE:
            StatusUseCase(
                ioService, command, listItemService, listService, configService, themeService, StatusService::PAUSED)
                .execute();
            return;
        case CommandList::BLOCKED:
            StatusUseCase(
                ioService, command, listItemService, listService, configService, themeService, StatusService::BLOCKED)
                .execute();
            return;
        case CommandList::COMPLETE:
            StatusUseCase(
                ioService, command, listItemService, listService, configService, themeService, StatusService::COMPLETED)
                .execute();
            return;
        case CommandList::CANCEL:
            StatusUseCase(
                ioService, command, listItemService, listService, configService, themeService, StatusService::CANCELLED)
                .execute();
            return;
        case CommandList::REMOVE:
            RemoveUseCase(ioService, command, listItemService, listService, configService, themeService).execute();
            return;
        case CommandList::ARCHIVE:
            ArchiveUseCase(ioService, command, listItemService, listService, configService, themeService).execute();
            return;
        case CommandList::RESTORE:
            RestoreUseCase(ioService, command, listItemService, listService, configService, themeService).execute();
            return;
        case CommandList::MOVE:
        case CommandList::COPY:
        case CommandList::DUPLICATE:
            MoveUseCase(ioService, command, commandService, listService, listItemService, configService, themeService)
                .execute();
            return;
        case CommandList::EMPTY:
            EmptyUseCase(ioService, command, listItemService, listService, configService).execute();
            return;
        case CommandList::CLEAN:
            CleanUseCase(ioService, command, listItemService, listService, configService, themeService).execute();
            return;
        case CommandList::LIST:
            ListUseCase(ioService,
                        command,
                        commandService,
                        listService,
                        listItemService,
                        fileStorageService,
                        configService,
                        themeService)
                .execute();
            return;
        case CommandList::USE:
            SwitchListUseCase(ioService,
                              command,
                              commandService,
                              listService,
                              listItemService,
                              fileStorageService,
                              configService,
                              themeService)
                .execute();
            return;
        case CommandList::STATS:
            StatsUseCase(ioService, command, configService, listItemService, themeService, listService).execute();
            return;
        case CommandList::RESET:
            ResetUseCase(ioService, command, listItemService, listService, configService, themeService).execute();
            return;
        default:
            help.commandNotFound();
            break;
    }
}
