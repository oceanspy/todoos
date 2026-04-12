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
                             HelpPrinter& help,
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
        case CommandRegistry::SHOW:
            ShowUseCase(
                ioService, help, commandService, command, configService, listService, listItemService, themeService)
                .execute();
            return;
        case CommandRegistry::ADD:
        case CommandRegistry::EDIT:
        case CommandRegistry::APPEND:
        case CommandRegistry::PREPEND:
        case CommandRegistry::DEADLINE:
            ListItemActionsUseCase(
                ioService, command, commandService, listItemService, listService, configService, themeService)
                .execute();
            return;
        case CommandRegistry::FIND:
            FindUseCase(ioService, command, configService, listService, listItemService, themeService).execute();
            return;
        case CommandRegistry::PRIORITY:
            PriorityUseCase(ioService, command, listItemService, listService, configService, themeService, "set")
                .execute();
            return;
        case CommandRegistry::INCREASE:
            PriorityUseCase(ioService, command, listItemService, listService, configService, themeService, "increase")
                .execute();
            return;
        case CommandRegistry::DECREASE:
            PriorityUseCase(ioService, command, listItemService, listService, configService, themeService, "decrease")
                .execute();
            return;
        case CommandRegistry::STATUS:
            StatusUseCase(ioService, command, listItemService, listService, configService, themeService, -1).execute();
            return;
        case CommandRegistry::TO_DO:
            StatusUseCase(
                ioService, command, listItemService, listService, configService, themeService, StatusService::TO_DO)
                .execute();
            return;
        case CommandRegistry::START:
            StatusUseCase(
                ioService, command, listItemService, listService, configService, themeService, StatusService::STARTED)
                .execute();
            return;
        case CommandRegistry::REVIEW:
            StatusUseCase(
                ioService, command, listItemService, listService, configService, themeService, StatusService::REVIEWING)
                .execute();
            return;
        case CommandRegistry::PAUSE:
            StatusUseCase(
                ioService, command, listItemService, listService, configService, themeService, StatusService::PAUSED)
                .execute();
            return;
        case CommandRegistry::BLOCKED:
            StatusUseCase(
                ioService, command, listItemService, listService, configService, themeService, StatusService::BLOCKED)
                .execute();
            return;
        case CommandRegistry::COMPLETE:
            StatusUseCase(
                ioService, command, listItemService, listService, configService, themeService, StatusService::COMPLETED)
                .execute();
            return;
        case CommandRegistry::CANCEL:
            StatusUseCase(
                ioService, command, listItemService, listService, configService, themeService, StatusService::CANCELLED)
                .execute();
            return;
        case CommandRegistry::REMOVE:
            RemoveUseCase(ioService, command, listItemService, listService, configService, themeService).execute();
            return;
        case CommandRegistry::ARCHIVE:
            ArchiveUseCase(ioService, command, listItemService, listService, configService, themeService).execute();
            return;
        case CommandRegistry::RESTORE:
            RestoreUseCase(ioService, command, listItemService, listService, configService, themeService).execute();
            return;
        case CommandRegistry::MOVE:
        case CommandRegistry::COPY:
        case CommandRegistry::DUPLICATE:
            MoveUseCase(ioService, command, commandService, listService, listItemService, configService, themeService)
                .execute();
            return;
        case CommandRegistry::EMPTY:
            EmptyUseCase(ioService, command, listItemService, listService, configService).execute();
            return;
        case CommandRegistry::CLEAN:
            CleanUseCase(ioService, command, listItemService, listService, configService, themeService).execute();
            return;
        case CommandRegistry::LIST:
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
        case CommandRegistry::USE:
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
        case CommandRegistry::STATS:
            StatsUseCase(ioService, command, configService, listItemService, themeService, listService).execute();
            return;
        case CommandRegistry::RESET:
            ResetUseCase(ioService, command, listItemService, listService, configService, themeService).execute();
            return;
        default:
            help.commandNotFound();
            break;
    }
}
