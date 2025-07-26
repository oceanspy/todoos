#include "src/CLIAutocomplete/CLIAutocompleteService.h"
#include "src/CLIController/CLIController.h"
#include "src/Command/Command.h"
#include "src/Command/CommandService.h"
#include "src/Command/CommandValidation.h"
#include "src/Config/ConfigService.h"
#include "src/FileDataStorage/CSVService.h"
#include "src/FileDataStorage/ConfService.h"
#include "src/FileDataStorage/JSONService.h"
#include "src/FileDataStorageRepositories/ConfigRepository.h"
#include "src/FileDataStorageRepositories/ListItemRepository.h"
#include "src/FileDataStorageRepositories/ListRepository.h"
#include "src/Help/Help.h"
#include "src/IOService/IOService.h"
#include "src/Init/Init.h"
#include "src/Init/Installation.h"
#include "src/List/ListItemService.h"
#include "src/List/ListService.h"

int
main(int argc, const char* argv[])
{
    std::string channel = "cli";
    IOService ioService = IOService(channel);
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
    if (installation.isNew()) {
        if (commandValidation.getCommandName() == "commands") {
            return 0;
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
    ConfigRepository cacheRepository = ConfigRepository(&confService, init.getCacheFilePath());
    ConfigService configService = ConfigService(ioService, init, configRepository, cacheRepository, command);

    FileStorageService fileStorageService = FileStorageService(ioService, configService);
    std::unique_ptr<FileDataServiceInterface> fileDataStorageServicePtr;
    if (configService.getFileDataStorageType() == "csv") {
        fileDataStorageServicePtr = std::make_unique<CSVService>(ioService);
    } else if (configService.getFileDataStorageType() == "json") {
        fileDataStorageServicePtr = std::make_unique<JSONService>(ioService);
    } else {
        ioService.error("File data storage type not supported.");
        return 1;
    }

    std::unique_ptr<FileDataServiceInterface> jsonFileDataStorageServicePtr = std::make_unique<JSONService>(ioService);
    EventBus bus = EventBus();
    PriorityService priorityService = PriorityService();
    StatusService statusService = StatusService();
    ListItemRepository listItemRepository =
        ListItemRepository(configService, fileDataStorageServicePtr.get(), priorityService, statusService);
    ListItemService listItemService =
        ListItemService(ioService, configService, listItemRepository, priorityService, statusService);
    ListRepository listRepository = ListRepository(configService, jsonFileDataStorageServicePtr.get());
    ListService listService = ListService(ioService, configService, listRepository, bus);

    bool autocomplete = false;
    try {
        CLIAutocompleteService cliAutocompleteService =
            CLIAutocompleteService(ioService, commandService, command, listService, listItemService);
        autocomplete = cliAutocompleteService.getCompletion();
    } catch (const std::exception& e) {
        // Just Quit
        return 1;
    }

    if (autocomplete) {
        return 0;
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
                                             cliThemeService);
    try {
        cliService.actions();
    } catch (ListNotFoundException& e) {
        help.listNotFound(e.getName());
        return 1;
    } catch (std::exception& e) {
        help.commandNotFound();
        return 1;
    }

    return 1;
}
