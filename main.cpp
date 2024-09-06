#include "src/Command/CommandValidation.h"
#include "src/Command/Command.h"
#include "src/Help/Help.h"
#include "src/IOService/IOService.h"
#include "src/Init/Init.h"
#include "src/Init/Installation.h"
#include "src/Config/ConfigService.h"
#include "src/FileDataStorageRepositories/ConfigRepository.h"
#include "src/CLIController/CLIController.h"

#include "src/CLIAutocomplete/CLIAutocompleteService.h"
#include "src/Command/CommandService.h"
#include "src/FileDataStorage/CSVService.h"
#include "src/FileDataStorage/JSONService.h"
#include "src/FileDataStorageRepositories/ListItemRepository.h"
#include "src/List/ListItemService.h"
#include "src/FileDataStorageRepositories/ListRepository.h"
#include "src/List/ListService.h"
#include "src/FileDataStorage/ConfService.h"


int main(int argc, const char *argv[])
{
    IOService ioService = IOService();
    Help help = Help(ioService);
    ConfService confService = ConfService(ioService);
    JSONService jsonService = JSONService(ioService);
    CSVService csvService = CSVService(ioService);

    CommandOption commandOption = CommandOption();
    CommandValidation commandValidation(commandOption, argc, argv);
    try {
        commandValidation.make();
    } catch (const std::exception& e) {
        if (commandValidation.getCommandName() == "commands") {
            return 1;
        }

        help.commandNotFound();
        return 1;
    }

    Init init = Init(ioService);
    Installation installation = Installation(ioService, jsonService, csvService, confService, init);
    if (installation.isNew())
    {
        if (commandValidation.getCommandName() == "commands")
        {
            return 1;
        }

        installation.make();
        ioService.br();
    }

    Command command(commandValidation.getCommandName(),
                    commandValidation.getCommandArguments(),
                    commandValidation.getCommandOptions(),
                    commandValidation.getRawCommand());
    CommandList commandList = CommandList();
    auto commandService = CommandService(commandList, commandOption);
    // Autocorrect command for common mistakes
    SmartCommand smartCommand = SmartCommand(command);
    command = smartCommand.apply();

    ConfigRepository configRepository = ConfigRepository(&confService, init.getConfigFilePath());
    ConfigService configService = ConfigService(ioService, init, configRepository, command);

    FileStorageService fileStorageService = FileStorageService(ioService, configService);
    std::unique_ptr<FileDataServiceInterface> fileDataStorageServicePtr;
    if (configService.getFileDataStorageType() == "csv")
    {
        fileDataStorageServicePtr = std::make_unique<CSVService>(ioService);
    } else if (configService.getFileDataStorageType() == "json")
    {
        fileDataStorageServicePtr = std::make_unique<JSONService>(ioService);
    } else
    {
        ioService.error("File data storage type not supported.");
        return 1;
    }

    std::unique_ptr<FileDataServiceInterface> jsonFileDataStorageServicePtr = std::make_unique<JSONService>(ioService);
    EventBus bus = EventBus();
    PriorityService priorityService = PriorityService();
    StatusService statusService = StatusService();
    ListItemRepository listItemRepository = ListItemRepository(configService, fileDataStorageServicePtr.get(), priorityService, statusService);
    ListItemService listItemService = ListItemService(ioService, configService, listItemRepository, priorityService, statusService, bus);
    ListRepository listRepository = ListRepository(configService, jsonFileDataStorageServicePtr.get());
    ListService listService = ListService(ioService, configService, listRepository, bus);

    // Checking if current list is valid:
    if (!CommandService::isCommand(command, "commands") &&
        !CommandService::isCommand(command, "use") &&
        !CommandService::isCommand(command, "list") &&
        !listService.isListExist(configService.getCurrentList())
    ) {
        ioService.br();
        ioService.error("List does not exist. Please use `todoos use {list}` to switch to a valid list.");
        return 1;
    }

    try {
        CLIAutocompleteService cliAutocompleteService = CLIAutocompleteService(ioService,
                                                                               commandService,
                                                                               command,
                                                                               listService,
                                                                               listItemService
                                                                               );
        if (cliAutocompleteService.getCompletion()) {
            return 0;
        }
    } catch (const std::exception& e) {
        // Just Quit
        return 1;
    }

    CLIThemeService cliThemeService = CLIThemeService(ioService, configService, listService, listItemService);
    CLIController cliService = CLIController(ioService,
                                             help,
                                             commandService,
                                             command,
                                             configService,
                                             fileStorageService,
                                             listService,
                                             listItemService,
                                             cliThemeService
                                             );
    cliService.actions();

    return 0;
}
