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
#include "../UseCase/UseUseCase.h"

CommandRouter::CommandRouter(IOService& ioService,
                             Help& help,
                             CommandService& commandService,
                             ConfigService& configService,
                             FileStorageService& fileStorageService,
                             ListService& listService,
                             ListItemService& listItemService,
                             CLIThemeService& cliThemeService)
  : ioService(ioService)
  , help(help)
  , commandService(commandService)
  , configService(configService)
  , fileStorageService(fileStorageService)
  , listService(listService)
  , listItemService(listItemService)
  , cliThemeService(cliThemeService)
{
}

void
CommandRouter::execute(Command& command)
{
    std::string commandName = CommandService::getCommandName(command.getName());
    std::map<std::string, std::string> options = command.getOptions();

    if (!CommandService::isCommandValidWithOption(CommandService::getCommandName(command.getName()),
                                                  command.getOptions())) {
        help.commandOptionNotSupported();
        return;
    }

    if (!commandService.isValid(commandName)) {
        help.commandNotFound();
        return;
    }

    switch (commandService.list().getCommandId(commandName)) {
        case CommandList::SHOW:
            ShowUseCase(
                ioService, help, commandService, command, configService, listService, listItemService, cliThemeService)
                .execute();
            return;
        case CommandList::ADD:
        case CommandList::EDIT:
        case CommandList::APPEND:
        case CommandList::PREPEND:
        case CommandList::DEADLINE:
            ListItemActionsUseCase(
                ioService, command, commandService, listItemService, listService, configService, cliThemeService)
                .execute();
            return;
        case CommandList::FIND:
            FindUseCase(ioService, command, configService, listService, listItemService, cliThemeService).execute();
            return;
        case CommandList::PRIORITY:
            PriorityUseCase(ioService, command, listItemService, listService, configService, cliThemeService, "set")
                .execute();
            return;
        case CommandList::INCREASE:
            PriorityUseCase(
                ioService, command, listItemService, listService, configService, cliThemeService, "increase")
                .execute();
            return;
        case CommandList::DECREASE:
            PriorityUseCase(
                ioService, command, listItemService, listService, configService, cliThemeService, "decrease")
                .execute();
            return;
        case CommandList::STATUS:
            StatusUseCase(ioService, command, listItemService, listService, configService, cliThemeService, -1)
                .execute();
            return;
        case CommandList::TO_DO:
            StatusUseCase(
                ioService, command, listItemService, listService, configService, cliThemeService, StatusService::TO_DO)
                .execute();
            return;
        case CommandList::START:
            StatusUseCase(ioService,
                          command,
                          listItemService,
                          listService,
                          configService,
                          cliThemeService,
                          StatusService::STARTED)
                .execute();
            return;
        case CommandList::REVIEW:
            StatusUseCase(ioService,
                          command,
                          listItemService,
                          listService,
                          configService,
                          cliThemeService,
                          StatusService::REVIEWING)
                .execute();
            return;
        case CommandList::PAUSE:
            StatusUseCase(
                ioService, command, listItemService, listService, configService, cliThemeService, StatusService::PAUSED)
                .execute();
            return;
        case CommandList::BLOCKED:
            StatusUseCase(ioService,
                          command,
                          listItemService,
                          listService,
                          configService,
                          cliThemeService,
                          StatusService::BLOCKED)
                .execute();
            return;
        case CommandList::COMPLETE:
            StatusUseCase(ioService,
                          command,
                          listItemService,
                          listService,
                          configService,
                          cliThemeService,
                          StatusService::COMPLETED)
                .execute();
            return;
        case CommandList::CANCEL:
            StatusUseCase(ioService,
                          command,
                          listItemService,
                          listService,
                          configService,
                          cliThemeService,
                          StatusService::CANCELLED)
                .execute();
            return;
        case CommandList::REMOVE:
            RemoveUseCase(ioService, command, listItemService, listService, configService, cliThemeService).execute();
            return;
        case CommandList::ARCHIVE:
            ArchiveUseCase(ioService, command, listItemService, listService, configService, cliThemeService).execute();
            return;
        case CommandList::RESTORE:
            RestoreUseCase(ioService, command, listItemService, listService, configService, cliThemeService).execute();
            return;
        case CommandList::MOVE:
        case CommandList::COPY:
        case CommandList::DUPLICATE:
            MoveUseCase(
                ioService, command, commandService, listService, listItemService, configService, cliThemeService)
                .execute();
            return;
        case CommandList::EMPTY:
            EmptyUseCase(ioService, command, listItemService, listService, configService).execute();
            return;
        case CommandList::CLEAN:
            CleanUseCase(ioService, command, listItemService, listService, configService, cliThemeService).execute();
            return;
        case CommandList::LIST:
            ListUseCase(ioService,
                        command,
                        commandService,
                        listService,
                        listItemService,
                        fileStorageService,
                        configService,
                        cliThemeService)
                .execute();
            return;
        case CommandList::USE:
            UseUseCase(ioService,
                       command,
                       commandService,
                       listService,
                       listItemService,
                       fileStorageService,
                       configService,
                       cliThemeService)
                .execute();
            return;
        case CommandList::STATS:
            StatsUseCase(ioService, command, configService, listItemService, cliThemeService, listService).execute();
            return;
        case CommandList::RESET:
            ResetUseCase(ioService, command, listItemService, listService, configService, cliThemeService).execute();
            return;
        default:
            help.commandNotFound();
            break;
    }
}
