#include "src/CLIAutocomplete/CLIAutocompleteService.h"
#include "src/CommandRouter/CommandRouter.h"
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
#include <clocale>

int
main(int argc, const char* argv[])
{
    std::setlocale(LC_CTYPE, "");

    // ----
    // Configuration initialization
    std::string channel = "cli";
    IOService ioService = IOService(channel);
    Help help = Help(ioService);
    ConfService confService = ConfService(ioService);
    JSONService jsonService = JSONService(ioService);
    CSVService csvService = CSVService(ioService);

    // ----
    // Input sanitization ACL
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
    Command command(commandValidation.getCommandName(),
                    commandValidation.getCommandArguments(),
                    commandValidation.getCommandOptions(),
                    commandValidation.getRawCommand());
    CommandList commandList = CommandList();
    auto commandService = CommandService(commandList, commandOption);

    // Autocorrect command for common mistakes
    SmartCommand smartCommand = SmartCommand(command);
    command = smartCommand.apply();

    // ----
    // Program installation
    Init init = Init(ioService);
    Installation installation = Installation(ioService, jsonService, csvService, confService, init);
    if (installation.isNew()) {
        if (commandValidation.getCommandName() == "commands") {
            return 0;
        }

        installation.make();
        ioService.br();
    }

    // ----
    // Configuration initialization
    ConfigRepository configRepository = ConfigRepository(&confService, init.getConfigFilePath());
    ConfigRepository cacheRepository = ConfigRepository(&confService, init.getCacheFilePath());
    ConfigService configService = ConfigService(ioService, init, configRepository, cacheRepository, command);

    // ----
    // Storage initialization
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

    // ----
    // Dependencies initialization
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

    // ----
    // Dealing with command autocomplete
    if (CommandService::isCommand(command, "commands")) {
        try {
            CLIAutocompleteService cliAutocompleteService =
                CLIAutocompleteService(ioService, commandService, command, listService, listItemService);
            cliAutocompleteService.getCompletion();
            return 1;
        } catch (const std::exception& e) {
            // Just Quit
            return 1;
        }
    }

    // ----
    // Initializing cli actions and frontend
    CLIThemeService cliThemeService = CLIThemeService(ioService, configService, listService, listItemService);
    CommandRouter commandRouter = CommandRouter(ioService,
                                                help,
                                                commandService,
                                                configService,
                                                fileStorageService,
                                                listService,
                                                listItemService,
                                                cliThemeService);

    // ----
    // Do the actions and print
    try {
        commandRouter.execute(command);
    } catch (ListNotFoundException& e) {
        help.listNotFound(e.getName());
        return 1;
    } catch (std::exception& e) {
        help.commandNotFound();
        return 1;
    }

    // ----
    // Keep the program opens until a button is pressed
    if (command.hasOption("no-quit")) {
        ioService.br();
        ioService.ask("Press any key to quit");
    }

    return 1;
}
