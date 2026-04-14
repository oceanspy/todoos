#include "CommandRouter.h"
#include "../UseCase/AddItemUseCase.h"
#include "../UseCase/AddListUseCase.h"
#include "../UseCase/AppendItemUseCase.h"
#include "../UseCase/ArchiveUseCase.h"
#include "../UseCase/CleanUseCase.h"
#include "../UseCase/CopyItemUseCase.h"
#include "../UseCase/CopyListUseCase.h"
#include "../UseCase/CurrentListUseCase.h"
#include "../UseCase/DeadlineItemUseCase.h"
#include "../UseCase/DuplicateItemUseCase.h"
#include "../UseCase/EditItemUseCase.h"
#include "../UseCase/EmptyUseCase.h"
#include "../UseCase/FindUseCase.h"
#include "../UseCase/MoveItemUseCase.h"
#include "../UseCase/MoveListUseCase.h"
#include "../UseCase/PrependItemUseCase.h"
#include "../UseCase/PriorityUseCase.h"
#include "../UseCase/RemoveListUseCase.h"
#include "../UseCase/RemoveUseCase.h"
#include "../UseCase/ResetUseCase.h"
#include "../UseCase/RestoreUseCase.h"
#include "../UseCase/ShowListUseCase.h"
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
    Command subCommand = commandService.getSubCommand(command);

    switch (commandService.list().getCommandId(CommandService::getCommandName(command.getName()))) {
        case CommandRegistry::SHOW:
            ShowUseCase(ioService, help, commandService, configService, listService, listItemService, themeService)
                .execute(command);
            return;
        case CommandRegistry::ADD:
            AddItemUseCase(ioService, commandService, listItemService, listService, configService, themeService)
                .execute(command);
            return;
        case CommandRegistry::EDIT:
            EditItemUseCase(ioService, commandService, listItemService, listService, configService, themeService)
                .execute(command);
            return;
        case CommandRegistry::APPEND:
            AppendItemUseCase(ioService, commandService, listItemService, listService, configService, themeService)
                .execute(command);
            return;
        case CommandRegistry::PREPEND:
            PrependItemUseCase(ioService, commandService, listItemService, listService, configService, themeService)
                .execute(command);
            return;
        case CommandRegistry::DEADLINE:
            DeadlineItemUseCase(ioService, commandService, listItemService, listService, configService, themeService)
                .execute(command);
            return;
        case CommandRegistry::FIND:
            FindUseCase(ioService, configService, listService, listItemService, themeService).execute(command);
            return;
        case CommandRegistry::PRIORITY:
            PriorityUseCase(ioService, listItemService, listService, configService, themeService)
                .execute(command, "set");
            return;
        case CommandRegistry::INCREASE:
            PriorityUseCase(ioService, listItemService, listService, configService, themeService)
                .execute(command, "increase");
            return;
        case CommandRegistry::DECREASE:
            PriorityUseCase(ioService, listItemService, listService, configService, themeService)
                .execute(command, "decrease");
            return;
        case CommandRegistry::STATUS:
            StatusUseCase(ioService, listItemService, listService, configService, themeService).execute(command, -1);
            return;
        case CommandRegistry::TO_DO:
            StatusUseCase(ioService, listItemService, listService, configService, themeService)
                .execute(command, StatusService::TO_DO);
            return;
        case CommandRegistry::START:
            StatusUseCase(ioService, listItemService, listService, configService, themeService)
                .execute(command, StatusService::STARTED);
            return;
        case CommandRegistry::REVIEW:
            StatusUseCase(ioService, listItemService, listService, configService, themeService)
                .execute(command, StatusService::REVIEWING);
            return;
        case CommandRegistry::PAUSE:
            StatusUseCase(ioService, listItemService, listService, configService, themeService)
                .execute(command, StatusService::PAUSED);
            return;
        case CommandRegistry::BLOCKED:
            StatusUseCase(ioService, listItemService, listService, configService, themeService)
                .execute(command, StatusService::BLOCKED);
            return;
        case CommandRegistry::COMPLETE:
            StatusUseCase(ioService, listItemService, listService, configService, themeService)
                .execute(command, StatusService::COMPLETED);
            return;
        case CommandRegistry::CANCEL:
            StatusUseCase(ioService, listItemService, listService, configService, themeService)
                .execute(command, StatusService::CANCELLED);
            return;
        case CommandRegistry::REMOVE:
            RemoveUseCase(ioService, listItemService, listService, configService, themeService).execute(command);
            return;
        case CommandRegistry::ARCHIVE:
            ArchiveUseCase(ioService, listItemService, listService, configService, themeService).execute(command);
            return;
        case CommandRegistry::RESTORE:
            RestoreUseCase(ioService, listItemService, listService, configService, themeService).execute(command);
            return;
        case CommandRegistry::MOVE:
            MoveItemUseCase(ioService, commandService, listService, listItemService, configService, themeService)
                .execute(command);
            return;
        case CommandRegistry::COPY:
            CopyItemUseCase(ioService, listService, listItemService, configService, themeService).execute(command);
            return;
        case CommandRegistry::DUPLICATE:
            DuplicateItemUseCase(ioService, listService, listItemService, configService, themeService).execute(command);
            return;
        case CommandRegistry::EMPTY:
            EmptyUseCase(ioService, listItemService, listService, configService, themeService).execute(command);
            return;
        case CommandRegistry::CLEAN:
            CleanUseCase(ioService, listItemService, listService, configService, themeService).execute(command);
            return;
        case CommandRegistry::LIST:

            if (commandService.isCommand(subCommand, "show")) {
                ShowListUseCase(ioService, listService, configService, themeService).execute(subCommand);
            } else if (commandService.isCommand(subCommand, "add")) {
                AddListUseCase(ioService, listService, fileStorageService, configService, themeService)
                    .execute(subCommand);
            } else if (commandService.isCommand(subCommand, "remove")) {
                RemoveListUseCase(ioService, listService, fileStorageService, configService, themeService)
                    .execute(subCommand);
            } else if (commandService.isCommand(subCommand, "move-to")) {
                MoveListUseCase(ioService,
                                commandService,
                                listService,
                                listItemService,
                                fileStorageService,
                                configService,
                                themeService)
                    .execute(subCommand);
            } else if (commandService.isCommand(subCommand, "copy")) {
                CopyListUseCase(
                    ioService, listService, listItemService, fileStorageService, configService, themeService)
                    .execute(subCommand);
            } else if (commandService.isCommand(subCommand, "current")) {
                CurrentListUseCase(ioService, configService).execute(subCommand);
            } else {
                help.commandNotFound();
            }

            return;
        case CommandRegistry::USE:
            SwitchListUseCase(ioService,
                              commandService,
                              listService,
                              listItemService,
                              fileStorageService,
                              configService,
                              themeService)
                .execute(command);
            return;
        case CommandRegistry::STATS:
            StatsUseCase(ioService, configService, listItemService, themeService, listService).execute(command);
            return;
        case CommandRegistry::RESET:
            ResetUseCase(ioService, listItemService, listService, configService, themeService).execute(command);
            return;
        default:
            help.commandNotFound();
            break;
    }
}
