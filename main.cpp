#include "src/Command/Command.h"
#include "src/Command/CommandRouter.h"
#include "src/Command/CommandService.h"
#include "src/Command/CommandValidation.h"
#include "src/Config/ConfigService.h"
#include "src/FileDataStorageRepositories/ConfigRepository.h"
#include "src/FileDataStorageRepositories/ListItemRepository.h"
#include "src/FileDataStorageRepositories/ListRepository.h"
#include "src/Help/HelpPrinter.h"
#include "src/IOService/IOService.h"
#include "src/Init/AppInitialization.h"
#include "src/Init/AppInstallation.h"
#include "src/List/ListItemService.h"
#include "src/List/ListService.h"
#include "src/Serializers/ConfSerializer.h"
#include "src/Serializers/CsvSerializer.h"
#include "src/Serializers/JsonSerializer.h"
#include "src/UseCase/CommandAutoCompleteUseCase.h"
#include <clocale>

int
main(int argc, const char* argv[])
{
    std::setlocale(LC_CTYPE, "");

    // ----
    // Configuration initialization
    std::string channel = "cli";
    IOService ioService = IOService(channel);
    HelpPrinter help = HelpPrinter(ioService);
    ConfSerializer confService = ConfSerializer(ioService);
    JsonSerializer jsonService = JsonSerializer(ioService);
    CsvSerializer csvService = CsvSerializer(ioService);

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
    CommandRegistry commandList = CommandRegistry();
    auto commandService = CommandService(commandList, commandOption);

    // ----
    // Autocorrect command for common mistakes or allowed shortcuts
    SmartCommand smartCommand = SmartCommand();
    command = smartCommand.apply(command);

    // ----
    // Verify command is valid
    if (!commandService.isValid(command)) {
        help.commandNotFoundSkipCommandAutocomplete(command);
        return 1;
    }

    if (!CommandService::isCommandValidWithOption(command)) {
        help.commandOptionNotSupportedSkipCommandAutocomplete(command);
        return 1;
    }

    // ----
    // Program initialization and first launch
    AppInitialization init = AppInitialization(ioService);
    AppInstallation installation = AppInstallation(ioService, jsonService, csvService, confService, init);
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
    DataSerializerInterface* fileDataStorageServicePtr;
    if (configService.getFileDataStorageType() == "csv") {
        fileDataStorageServicePtr = &csvService;
    } else if (configService.getFileDataStorageType() == "json") {
        fileDataStorageServicePtr = &jsonService;
    } else {
        ioService.error("File data storage type not supported.");
        return 1;
    }

    // ----
    // Dependencies initialization
    EventBus bus = EventBus();
    PriorityService priorityService = PriorityService();
    StatusService statusService = StatusService();
    ListItemRepository listItemRepository =
        ListItemRepository(configService, fileDataStorageServicePtr, priorityService, statusService);
    ListItemService listItemService =
        ListItemService(ioService, configService, listItemRepository, priorityService, statusService);
    ListRepository listRepository = ListRepository(configService, &jsonService);
    ListService listService = ListService(ioService, configService, listRepository, bus);

    // ----
    // Dealing with command autocomplete
    if (CommandService::isCommand(command, "commands")) {
        try {
            CommandAutoCompleteUseCase(ioService, commandService, listService, listItemService).execute(command);
            return 1;
        } catch (const std::exception& e) {
            // Just Quit
            return 1;
        }
    }

    // ----
    // Initializing cli actions and frontend
    ThemeService themeService = ThemeService(ioService, configService, listService, listItemService);
    CommandRouter commandRouter = CommandRouter(
        ioService, help, commandService, configService, fileStorageService, listService, listItemService, themeService);

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
