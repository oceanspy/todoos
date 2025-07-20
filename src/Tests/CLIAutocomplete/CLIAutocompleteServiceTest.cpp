#include "../../CLIAutocomplete/CLIAutocompleteService.h"
#include "../../FileDataStorage/ConfService.h"
#include "../../FileDataStorage/JSONService.h"
#include "../../FileDataStorageRepositories/ListItemRepository.h"
#include "../../IOService/IOService.h"
#include "../../List/ListItemService.h"
#include "../Mock/MockInit.h"
#include "../Mock/MockInstallation.h"
#include <catch2/catch_test_macros.hpp>
#include <fstream>
#include <iostream>

std::string
captureGetCompletionOutput(CLIAutocompleteService& autocompleteService)
{
    std::ostringstream oss;
    std::streambuf* coutBuffer = std::cout.rdbuf();
    std::cout.rdbuf(oss.rdbuf());

    // Call getCompletion()
    autocompleteService.getCompletion();

    // Restore cout buffer
    std::cout.rdbuf(coutBuffer);

    // Get the printed output
    return oss.str();
}

TEST_CASE("CLIAutocompleteService Tests", "[CLIAutocompleteService]")
{
    // Create mock objects
    std::string channel = "cli";
    IOService ioService(channel);
    ConfService confService = ConfService(ioService);
    JSONService jsonService = JSONService(ioService);
    std::unique_ptr<FileDataServiceInterface> fileDataStorageServicePtr = std::make_unique<JSONService>(ioService);
    std::unique_ptr<FileDataServiceInterface> fileDataConfigStorageServicePtr =
        std::make_unique<ConfService>(ioService);
    MockInit init(ioService, "_todoos_CLIAutocompleteServiceTest");
    MockInstallation installation(ioService, jsonService, confService, init);
    std::string tempListName = "tempListName";
    std::string tempList2Name = "tempList2Name";
    installation.make();
    EventBus bus = EventBus();
    CommandList commandList;
    CommandShortcut commandShortcutService = CommandShortcut();
    CommandOption commandOption = CommandOption();
    auto commandService = CommandService(commandList, commandOption);

    SECTION("Autocomplete commands: commands")
    {
        Command command = Command("commands", {}, {}, "commands");

        ConfigRepository configRepository(fileDataConfigStorageServicePtr.get(), init.getConfigFilePath());
        ConfigRepository cacheRepository(fileDataConfigStorageServicePtr.get(), init.getCacheFilePath());
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService = PriorityService();
        StatusService statusService = StatusService();
        ListItemRepository listItemRepository(
            configService, fileDataStorageServicePtr.get(), priorityService, statusService);
        ListItemService listItemService(
            ioService, configService, listItemRepository, priorityService, statusService, bus);
        ListRepository listRepository(configService, fileDataStorageServicePtr.get());
        ListService listService(ioService, configService, listRepository, bus);

        // Create an instance of CLIAutocompleteService
        CLIAutocompleteService autocompleteService(ioService, commandService, command, listService, listItemService);

        // Test getCompletion function
        REQUIRE_NOTHROW(autocompleteService.getCompletion());

        // Capture the printed output of getCompletion()
        std::string printedString = captureGetCompletionOutput(autocompleteService);
        std::string expectedString = commandService.getMainCommandListAsString() + "\n";
        REQUIRE(printedString == expectedString);
    }

    SECTION("Autocomplete commands: commands add")
    {
        Command command = Command("commands", { "add" }, {}, "commands add");

        ConfigRepository configRepository(fileDataConfigStorageServicePtr.get(), init.getConfigFilePath());
        ConfigRepository cacheRepository(fileDataConfigStorageServicePtr.get(), init.getCacheFilePath());
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService = PriorityService();
        StatusService statusService = StatusService();
        ListItemRepository listItemRepository(
            configService, fileDataStorageServicePtr.get(), priorityService, statusService);
        ListItemService listItemService(
            ioService, configService, listItemRepository, priorityService, statusService, bus);
        ListRepository listRepository(configService, fileDataStorageServicePtr.get());
        ListService listService(ioService, configService, listRepository, bus);

        // Create an instance of CLIAutocompleteService
        CLIAutocompleteService autocompleteService(ioService, commandService, command, listService, listItemService);

        // Test getCompletion function
        REQUIRE_NOTHROW(autocompleteService.getCompletion());

        // Capture the printed output of getCompletion()
        std::string printedString = captureGetCompletionOutput(autocompleteService);
        std::string expectedString = "";
        REQUIRE(printedString == expectedString);
    }

    SECTION("Autocomplete commands: commands add -p urg")
    {
        Command command = Command("commands", { "add" }, { { "priority", "urg" } }, "commands add -p urg");

        ConfigRepository configRepository(fileDataConfigStorageServicePtr.get(), init.getConfigFilePath());
        ConfigRepository cacheRepository(fileDataConfigStorageServicePtr.get(), init.getCacheFilePath());
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService = PriorityService();
        StatusService statusService = StatusService();
        ListItemRepository listItemRepository(
            configService, fileDataStorageServicePtr.get(), priorityService, statusService);
        ListItemService listItemService(
            ioService, configService, listItemRepository, priorityService, statusService, bus);
        ListRepository listRepository(configService, fileDataStorageServicePtr.get());
        ListService listService(ioService, configService, listRepository, bus);

        // Create an instance of CLIAutocompleteService
        CLIAutocompleteService autocompleteService(ioService, commandService, command, listService, listItemService);

        // Test getCompletion function
        REQUIRE_NOTHROW(autocompleteService.getCompletion());

        // Capture the printed output of getCompletion()
        std::string printedString = captureGetCompletionOutput(autocompleteService);
        std::string expectedString = priorityService.getNamesAsString() + "\n";
        REQUIRE(printedString == expectedString);
    }

    SECTION("Autocomplete commands: commands add -p urgent")
    {
        Command command = Command("commands", { "add" }, { { "priority", "urgent" } }, "commands add -p urgent");

        ConfigRepository configRepository(fileDataConfigStorageServicePtr.get(), init.getConfigFilePath());
        ConfigRepository cacheRepository(fileDataConfigStorageServicePtr.get(), init.getCacheFilePath());
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService = PriorityService();
        StatusService statusService = StatusService();
        ListItemRepository listItemRepository(
            configService, fileDataStorageServicePtr.get(), priorityService, statusService);
        ListItemService listItemService(
            ioService, configService, listItemRepository, priorityService, statusService, bus);
        ListRepository listRepository(configService, fileDataStorageServicePtr.get());
        ListService listService(ioService, configService, listRepository, bus);

        // Create an instance of CLIAutocompleteService
        CLIAutocompleteService autocompleteService(ioService, commandService, command, listService, listItemService);

        // Test getCompletion function
        REQUIRE_NOTHROW(autocompleteService.getCompletion());

        // Capture the printed output of getCompletion()
        std::string printedString = captureGetCompletionOutput(autocompleteService);
        std::string expectedString = "";
        REQUIRE(printedString == expectedString);
    }

    SECTION("Autocomplete commands: commands add -s")
    {
        Command command = Command("commands", { "add" }, { { "status", "" } }, "commands add -s");

        ConfigRepository configRepository(fileDataConfigStorageServicePtr.get(), init.getConfigFilePath());
        ConfigRepository cacheRepository(fileDataConfigStorageServicePtr.get(), init.getCacheFilePath());
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService = PriorityService();
        StatusService statusService = StatusService();
        ListItemRepository listItemRepository(
            configService, fileDataStorageServicePtr.get(), priorityService, statusService);
        ListItemService listItemService(
            ioService, configService, listItemRepository, priorityService, statusService, bus);
        ListRepository listRepository(configService, fileDataStorageServicePtr.get());
        ListService listService(ioService, configService, listRepository, bus);

        // Create an instance of CLIAutocompleteService
        CLIAutocompleteService autocompleteService(ioService, commandService, command, listService, listItemService);

        // Test getCompletion function
        REQUIRE_NOTHROW(autocompleteService.getCompletion());

        // Capture the printed output of getCompletion()
        std::string printedString = captureGetCompletionOutput(autocompleteService);
        std::string expectedString = statusService.getCommandNamesAsString() + "\n";
        REQUIRE(printedString == expectedString);
    }

    SECTION("Autocomplete commands: commands add -s to-")
    {
        Command command = Command("commands", { "add" }, { { "status", "to-" } }, "commands add -s to-");

        ConfigRepository configRepository(fileDataConfigStorageServicePtr.get(), init.getConfigFilePath());
        ConfigRepository cacheRepository(fileDataConfigStorageServicePtr.get(), init.getCacheFilePath());
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService = PriorityService();
        StatusService statusService = StatusService();
        ListItemRepository listItemRepository(
            configService, fileDataStorageServicePtr.get(), priorityService, statusService);
        ListItemService listItemService(
            ioService, configService, listItemRepository, priorityService, statusService, bus);
        ListRepository listRepository(configService, fileDataStorageServicePtr.get());
        ListService listService(ioService, configService, listRepository, bus);

        // Create an instance of CLIAutocompleteService
        CLIAutocompleteService autocompleteService(ioService, commandService, command, listService, listItemService);

        // Test getCompletion function
        REQUIRE_NOTHROW(autocompleteService.getCompletion());

        // Capture the printed output of getCompletion()
        std::string printedString = captureGetCompletionOutput(autocompleteService);
        std::string expectedString = statusService.getCommandNamesAsString() + "\n";
        REQUIRE(printedString == expectedString);
    }

    SECTION("Autocomplete commands: commands add -s to-do")
    {
        Command command = Command("commands", { "add" }, { { "status", "to-do" } }, "commands add -s to-do");

        ConfigRepository configRepository(fileDataConfigStorageServicePtr.get(), init.getConfigFilePath());
        ConfigRepository cacheRepository(fileDataConfigStorageServicePtr.get(), init.getCacheFilePath());
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService = PriorityService();
        StatusService statusService = StatusService();
        ListItemRepository listItemRepository(
            configService, fileDataStorageServicePtr.get(), priorityService, statusService);
        ListItemService listItemService(
            ioService, configService, listItemRepository, priorityService, statusService, bus);
        ListRepository listRepository(configService, fileDataStorageServicePtr.get());
        ListService listService(ioService, configService, listRepository, bus);

        // Create an instance of CLIAutocompleteService
        CLIAutocompleteService autocompleteService(ioService, commandService, command, listService, listItemService);

        // Test getCompletion function
        REQUIRE_NOTHROW(autocompleteService.getCompletion());

        // Capture the printed output of getCompletion()
        std::string printedString = captureGetCompletionOutput(autocompleteService);
        std::string expectedString = "";
        REQUIRE(printedString == expectedString);
    }

    SECTION("Autocomplete commands: commands add ceci est un test")
    {
        Command command =
            Command("commands", { "add", "ceci", "est", "un", "test" }, {}, "commands add ceci est un test");

        ConfigRepository configRepository(fileDataConfigStorageServicePtr.get(), init.getConfigFilePath());
        ConfigRepository cacheRepository(fileDataConfigStorageServicePtr.get(), init.getCacheFilePath());
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService = PriorityService();
        StatusService statusService = StatusService();
        ListItemRepository listItemRepository(
            configService, fileDataStorageServicePtr.get(), priorityService, statusService);
        ListItemService listItemService(
            ioService, configService, listItemRepository, priorityService, statusService, bus);
        ListRepository listRepository(configService, fileDataStorageServicePtr.get());
        ListService listService(ioService, configService, listRepository, bus);

        // Create an instance of CLIAutocompleteService
        CLIAutocompleteService autocompleteService(ioService, commandService, command, listService, listItemService);

        // Test getCompletion function
        REQUIRE_NOTHROW(autocompleteService.getCompletion());

        // Capture the printed output of getCompletion()
        std::string printedString = captureGetCompletionOutput(autocompleteService);
        std::string expectedString = "";
        REQUIRE(printedString == expectedString);
    }

    SECTION("Autocomplete commands: commands add with list")
    {
        Command command =
            Command("commands", { "add" }, { { "list", "tempListName" } }, "commands -l tempListName add");

        ConfigRepository configRepository(fileDataConfigStorageServicePtr.get(), init.getConfigFilePath());
        ConfigRepository cacheRepository(fileDataConfigStorageServicePtr.get(), init.getCacheFilePath());
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService = PriorityService();
        StatusService statusService = StatusService();
        ListItemRepository listItemRepository(
            configService, fileDataStorageServicePtr.get(), priorityService, statusService);
        ListItemService listItemService(
            ioService, configService, listItemRepository, priorityService, statusService, bus);
        ListRepository listRepository(configService, fileDataStorageServicePtr.get());
        ListService listService(ioService, configService, listRepository, bus);

        // Create an instance of CLIAutocompleteService
        CLIAutocompleteService autocompleteService(ioService, commandService, command, listService, listItemService);

        // Test getCompletion function
        REQUIRE_NOTHROW(autocompleteService.getCompletion());

        // Capture the printed output of getCompletion()
        std::string printedString = captureGetCompletionOutput(autocompleteService);
        std::string expectedString = "";
        REQUIRE(printedString == expectedString);
    }

    SECTION("Autocomplete commands: commands add with list and priority")
    {
        Command command = Command("commands",
                                  { "add" },
                                  { { "list", "tempListName" }, { "priority", "high" } },
                                  "commands -l tempListName -p high add");

        ConfigRepository configRepository(fileDataConfigStorageServicePtr.get(), init.getConfigFilePath());
        ConfigRepository cacheRepository(fileDataConfigStorageServicePtr.get(), init.getCacheFilePath());
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService = PriorityService();
        StatusService statusService = StatusService();
        ListItemRepository listItemRepository(
            configService, fileDataStorageServicePtr.get(), priorityService, statusService);
        ListItemService listItemService(
            ioService, configService, listItemRepository, priorityService, statusService, bus);
        ListRepository listRepository(configService, fileDataStorageServicePtr.get());
        ListService listService(ioService, configService, listRepository, bus);

        // Create an instance of CLIAutocompleteService
        CLIAutocompleteService autocompleteService(ioService, commandService, command, listService, listItemService);

        // Test getCompletion function
        REQUIRE_NOTHROW(autocompleteService.getCompletion());

        // Capture the printed output of getCompletion()
        std::string printedString = captureGetCompletionOutput(autocompleteService);
        std::string expectedString = "";
        REQUIRE(printedString == expectedString);
    }

    SECTION("Autocomplete commands: commands -l tempListName")
    {
        Command command = Command("commands", {}, { { "list", "tempListName" } }, "commands -l tempListName");

        ConfigRepository configRepository(fileDataConfigStorageServicePtr.get(), init.getConfigFilePath());
        ConfigRepository cacheRepository(fileDataConfigStorageServicePtr.get(), init.getCacheFilePath());
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService = PriorityService();
        StatusService statusService = StatusService();
        ListItemRepository listItemRepository(
            configService, fileDataStorageServicePtr.get(), priorityService, statusService);
        ListItemService listItemService(
            ioService, configService, listItemRepository, priorityService, statusService, bus);
        ListRepository listRepository(configService, fileDataStorageServicePtr.get());
        ListService listService(ioService, configService, listRepository, bus);

        // Create an instance of CLIAutocompleteService
        CLIAutocompleteService autocompleteService(ioService, commandService, command, listService, listItemService);

        // Test getCompletion function
        REQUIRE_NOTHROW(autocompleteService.getCompletion());

        // Capture the printed output of getCompletion()
        std::string printedString = captureGetCompletionOutput(autocompleteService);
        std::string expectedString = commandService.getMainCommandListAsString() + "\n";
        REQUIRE(printedString == expectedString);
    }

    SECTION("Autocomplete commands: commands add -l test2")
    {
        Command command = Command("commands", { "add" }, { { "list", "test2" } }, "commands add -l test2");

        ConfigRepository configRepository(fileDataConfigStorageServicePtr.get(), init.getConfigFilePath());
        ConfigRepository cacheRepository(fileDataConfigStorageServicePtr.get(), init.getCacheFilePath());
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService = PriorityService();
        StatusService statusService = StatusService();
        ListItemRepository listItemRepository(
            configService, fileDataStorageServicePtr.get(), priorityService, statusService);
        ListItemService listItemService(
            ioService, configService, listItemRepository, priorityService, statusService, bus);
        ListRepository listRepository(configService, fileDataStorageServicePtr.get());
        ListService listService(ioService, configService, listRepository, bus);

        // Create an instance of CLIAutocompleteService
        CLIAutocompleteService autocompleteService(ioService, commandService, command, listService, listItemService);

        // Test getCompletion function
        REQUIRE_NOTHROW(autocompleteService.getCompletion());

        // Capture the printed output of getCompletion()
        std::string printedString = captureGetCompletionOutput(autocompleteService);
        std::string expectedString = "tempList2Name tempListName\n";
        REQUIRE(printedString == expectedString);
    }

    SECTION("Autocomplete commands: commands add -l wrongList")
    {
        Command command = Command("commands", { "add" }, { { "list", "wrongList" } }, "commands -l wrongList add");

        ConfigRepository configRepository(fileDataConfigStorageServicePtr.get(), init.getConfigFilePath());
        ConfigRepository cacheRepository(fileDataConfigStorageServicePtr.get(), init.getCacheFilePath());
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService = PriorityService();
        StatusService statusService = StatusService();
        ListItemRepository listItemRepository(
            configService, fileDataStorageServicePtr.get(), priorityService, statusService);
        ListItemService listItemService(
            ioService, configService, listItemRepository, priorityService, statusService, bus);
        ListRepository listRepository(configService, fileDataStorageServicePtr.get());
        ListService listService(ioService, configService, listRepository, bus);

        // Create an instance of CLIAutocompleteService
        CLIAutocompleteService autocompleteService(ioService, commandService, command, listService, listItemService);

        // Test getCompletion function
        REQUIRE_NOTHROW(autocompleteService.getCompletion());

        // Capture the printed output of getCompletion()
        std::string printedString = captureGetCompletionOutput(autocompleteService);
        std::string expectedString = "tempList2Name tempListName\n";
        REQUIRE(printedString == expectedString);
    }

    SECTION("Autocomplete commands: commands edit")
    {
        Command command = Command("commands", { "edit" }, {}, "commands edit");

        ConfigRepository configRepository(fileDataConfigStorageServicePtr.get(), init.getConfigFilePath());
        ConfigRepository cacheRepository(fileDataConfigStorageServicePtr.get(), init.getCacheFilePath());
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService = PriorityService();
        StatusService statusService = StatusService();
        ListItemRepository listItemRepository(
            configService, fileDataStorageServicePtr.get(), priorityService, statusService);
        ListItemService listItemService(
            ioService, configService, listItemRepository, priorityService, statusService, bus);
        ListRepository listRepository(configService, fileDataStorageServicePtr.get());
        ListService listService(ioService, configService, listRepository, bus);

        // Create an instance of CLIAutocompleteService
        CLIAutocompleteService autocompleteService(ioService, commandService, command, listService, listItemService);

        // Test getCompletion function
        REQUIRE_NOTHROW(autocompleteService.getCompletion());

        // Capture the printed output of getCompletion()
        std::string printedString = captureGetCompletionOutput(autocompleteService);
        std::string expectedString = "aaaa bbbb\n";
        REQUIRE(printedString == expectedString);
    }

    SECTION("Autocomplete commands: commands edit aaaa")
    {
        Command command = Command("commands", { "edit", "aaaa" }, {}, "commands edit aaaa");

        ConfigRepository configRepository(fileDataConfigStorageServicePtr.get(), init.getConfigFilePath());
        ConfigRepository cacheRepository(fileDataConfigStorageServicePtr.get(), init.getCacheFilePath());
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService = PriorityService();
        StatusService statusService = StatusService();
        ListItemRepository listItemRepository(
            configService, fileDataStorageServicePtr.get(), priorityService, statusService);
        ListItemService listItemService(
            ioService, configService, listItemRepository, priorityService, statusService, bus);
        ListRepository listRepository(configService, fileDataStorageServicePtr.get());
        ListService listService(ioService, configService, listRepository, bus);

        // Create an instance of CLIAutocompleteService
        CLIAutocompleteService autocompleteService(ioService, commandService, command, listService, listItemService);

        // Test getCompletion function
        REQUIRE_NOTHROW(autocompleteService.getCompletion());

        // Capture the printed output of getCompletion()
        std::string printedString = captureGetCompletionOutput(autocompleteService);
        std::string expectedString = "\"test\\ 1\"\n";
        REQUIRE(printedString == expectedString);
    }

    SECTION("Autocomplete commands: commands edit aaaa test")
    {
        Command command = Command("commands", { "edit", "aaaa", "test" }, {}, "commands edit aaaa test");

        ConfigRepository configRepository(fileDataConfigStorageServicePtr.get(), init.getConfigFilePath());
        ConfigRepository cacheRepository(fileDataConfigStorageServicePtr.get(), init.getCacheFilePath());
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService = PriorityService();
        StatusService statusService = StatusService();
        ListItemRepository listItemRepository(
            configService, fileDataStorageServicePtr.get(), priorityService, statusService);
        ListItemService listItemService(
            ioService, configService, listItemRepository, priorityService, statusService, bus);
        ListRepository listRepository(configService, fileDataStorageServicePtr.get());
        ListService listService(ioService, configService, listRepository, bus);

        // Create an instance of CLIAutocompleteService
        CLIAutocompleteService autocompleteService(ioService, commandService, command, listService, listItemService);

        // Test getCompletion function
        REQUIRE_NOTHROW(autocompleteService.getCompletion());

        // Capture the printed output of getCompletion()
        std::string printedString = captureGetCompletionOutput(autocompleteService);
        std::string expectedString = "\"test\\ 1\"\n";
        REQUIRE(printedString == expectedString);
    }

    SECTION("Autocomplete commands: commands wrong")
    {
        Command command = Command("commands", { "wrong" }, {}, "commands wrong");

        ConfigRepository configRepository(fileDataConfigStorageServicePtr.get(), init.getConfigFilePath());
        ConfigRepository cacheRepository(fileDataConfigStorageServicePtr.get(), init.getCacheFilePath());
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService = PriorityService();
        StatusService statusService = StatusService();
        ListItemRepository listItemRepository(
            configService, fileDataStorageServicePtr.get(), priorityService, statusService);
        ListItemService listItemService(
            ioService, configService, listItemRepository, priorityService, statusService, bus);
        ListRepository listRepository(configService, fileDataStorageServicePtr.get());
        ListService listService(ioService, configService, listRepository, bus);

        // Create an instance of CLIAutocompleteService
        CLIAutocompleteService autocompleteService(ioService, commandService, command, listService, listItemService);

        // Test getCompletion function
        REQUIRE_NOTHROW(autocompleteService.getCompletion());

        // Capture the printed output of getCompletion()
        std::string printedString = captureGetCompletionOutput(autocompleteService);
        std::string expectedString = "";
        REQUIRE(printedString == expectedString);
    }

    SECTION("Autocomplete commands: commands -l")
    {
        Command command = Command("commands", {}, { { "list", "" } }, "commands -l");

        ConfigRepository configRepository(fileDataConfigStorageServicePtr.get(), init.getConfigFilePath());
        ConfigRepository cacheRepository(fileDataConfigStorageServicePtr.get(), init.getCacheFilePath());
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService = PriorityService();
        StatusService statusService = StatusService();
        ListItemRepository listItemRepository(
            configService, fileDataStorageServicePtr.get(), priorityService, statusService);
        ListItemService listItemService(
            ioService, configService, listItemRepository, priorityService, statusService, bus);
        ListRepository listRepository(configService, fileDataStorageServicePtr.get());
        ListService listService(ioService, configService, listRepository, bus);

        // Create an instance of CLIAutocompleteService
        CLIAutocompleteService autocompleteService(ioService, commandService, command, listService, listItemService);

        // Test getCompletion function
        REQUIRE_NOTHROW(autocompleteService.getCompletion());

        // Capture the printed output of getCompletion()
        std::string printedString = captureGetCompletionOutput(autocompleteService);
        std::string expectedString = "tempList2Name tempListName\n";
        REQUIRE(printedString == expectedString);
    }

    SECTION("Autocomplete commands: commands print")
    {
        Command command = Command("commands", { "print" }, {}, "commands print");

        ConfigRepository configRepository(fileDataConfigStorageServicePtr.get(), init.getConfigFilePath());
        ConfigRepository cacheRepository(fileDataConfigStorageServicePtr.get(), init.getCacheFilePath());
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService = PriorityService();
        StatusService statusService = StatusService();
        ListItemRepository listItemRepository(
            configService, fileDataStorageServicePtr.get(), priorityService, statusService);
        ListItemService listItemService(
            ioService, configService, listItemRepository, priorityService, statusService, bus);
        ListRepository listRepository(configService, fileDataStorageServicePtr.get());
        ListService listService(ioService, configService, listRepository, bus);

        // Create an instance of CLIAutocompleteService
        CLIAutocompleteService autocompleteService(ioService, commandService, command, listService, listItemService);

        // Test getCompletion function
        REQUIRE_NOTHROW(autocompleteService.getCompletion());

        // Capture the printed output of getCompletion()
        std::string printedString = captureGetCompletionOutput(autocompleteService);
        std::string expectedString = "";
        REQUIRE(printedString == expectedString);
    }

    SECTION("Autocomplete commands: commands list")
    {
        Command command = Command("commands", { "list" }, {}, "commands list");

        ConfigRepository configRepository(fileDataConfigStorageServicePtr.get(), init.getConfigFilePath());
        ConfigRepository cacheRepository(fileDataConfigStorageServicePtr.get(), init.getCacheFilePath());
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService = PriorityService();
        StatusService statusService = StatusService();
        ListItemRepository listItemRepository(
            configService, fileDataStorageServicePtr.get(), priorityService, statusService);
        ListItemService listItemService(
            ioService, configService, listItemRepository, priorityService, statusService, bus);
        ListRepository listRepository(configService, fileDataStorageServicePtr.get());
        ListService listService(ioService, configService, listRepository, bus);

        // Create an instance of CLIAutocompleteService
        CLIAutocompleteService autocompleteService(ioService, commandService, command, listService, listItemService);

        // Test getCompletion function
        REQUIRE_NOTHROW(autocompleteService.getCompletion());

        // Capture the printed output of getCompletion()
        std::string printedString = captureGetCompletionOutput(autocompleteService);
        std::string expectedString = "add rename remove show copy\n";
        REQUIRE(printedString == expectedString);
    }

    SECTION("Autocomplete commands: commands list add")
    {
        Command command = Command("commands", { "list", "add" }, {}, "commands list add");

        ConfigRepository configRepository(fileDataConfigStorageServicePtr.get(), init.getConfigFilePath());
        ConfigRepository cacheRepository(fileDataConfigStorageServicePtr.get(), init.getCacheFilePath());
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService = PriorityService();
        StatusService statusService = StatusService();
        ListItemRepository listItemRepository(
            configService, fileDataStorageServicePtr.get(), priorityService, statusService);
        ListItemService listItemService(
            ioService, configService, listItemRepository, priorityService, statusService, bus);
        ListRepository listRepository(configService, fileDataStorageServicePtr.get());
        ListService listService(ioService, configService, listRepository, bus);

        // Create an instance of CLIAutocompleteService
        CLIAutocompleteService autocompleteService(ioService, commandService, command, listService, listItemService);

        // Test getCompletion function
        REQUIRE_NOTHROW(autocompleteService.getCompletion());

        // Capture the printed output of getCompletion()
        std::string printedString = captureGetCompletionOutput(autocompleteService);
        std::string expectedString = "";
        REQUIRE(printedString == expectedString);
    }

    SECTION("Autocomplete commands: commands list rename")
    {
        Command command = Command("commands", { "list", "rename" }, {}, "commands list rename");

        ConfigRepository configRepository(fileDataConfigStorageServicePtr.get(), init.getConfigFilePath());
        ConfigRepository cacheRepository(fileDataConfigStorageServicePtr.get(), init.getCacheFilePath());
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService = PriorityService();
        StatusService statusService = StatusService();
        ListItemRepository listItemRepository(
            configService, fileDataStorageServicePtr.get(), priorityService, statusService);
        ListItemService listItemService(
            ioService, configService, listItemRepository, priorityService, statusService, bus);
        ListRepository listRepository(configService, fileDataStorageServicePtr.get());
        ListService listService(ioService, configService, listRepository, bus);

        // Create an instance of CLIAutocompleteService
        CLIAutocompleteService autocompleteService(ioService, commandService, command, listService, listItemService);

        // Test getCompletion function
        REQUIRE_NOTHROW(autocompleteService.getCompletion());

        // Capture the printed output of getCompletion()
        std::string printedString = captureGetCompletionOutput(autocompleteService);
        std::string expectedString = "tempList2Name tempListName\n";
        REQUIRE(printedString == expectedString);
    }

    SECTION("Autocomplete commands: commands list rename tempListName")
    {
        Command command =
            Command("commands", { "list", "rename", "tempListName" }, {}, "commands list rename tempListName");

        ConfigRepository configRepository(fileDataConfigStorageServicePtr.get(), init.getConfigFilePath());
        ConfigRepository cacheRepository(fileDataConfigStorageServicePtr.get(), init.getCacheFilePath());
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService = PriorityService();
        StatusService statusService = StatusService();
        ListItemRepository listItemRepository(
            configService, fileDataStorageServicePtr.get(), priorityService, statusService);
        ListItemService listItemService(
            ioService, configService, listItemRepository, priorityService, statusService, bus);
        ListRepository listRepository(configService, fileDataStorageServicePtr.get());
        ListService listService(ioService, configService, listRepository, bus);

        // Create an instance of CLIAutocompleteService
        CLIAutocompleteService autocompleteService(ioService, commandService, command, listService, listItemService);

        // Test getCompletion function
        REQUIRE_NOTHROW(autocompleteService.getCompletion());

        // Capture the printed output of getCompletion()
        std::string printedString = captureGetCompletionOutput(autocompleteService);
        std::string expectedString = "";
        REQUIRE(printedString == expectedString);
    }

    SECTION("Autocomplete commands: commands list rename tempListName -l tempList2Name")
    {
        Command command = Command("commands",
                                  { "list", "rename", "tempListName" },
                                  { { "list", "tempList2Name" } },
                                  "commands list rename tempListName -l tempList2Name");

        ConfigRepository configRepository(fileDataConfigStorageServicePtr.get(), init.getConfigFilePath());
        ConfigRepository cacheRepository(fileDataConfigStorageServicePtr.get(), init.getCacheFilePath());
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService = PriorityService();
        StatusService statusService = StatusService();
        ListItemRepository listItemRepository(
            configService, fileDataStorageServicePtr.get(), priorityService, statusService);
        ListItemService listItemService(
            ioService, configService, listItemRepository, priorityService, statusService, bus);
        ListRepository listRepository(configService, fileDataStorageServicePtr.get());
        ListService listService(ioService, configService, listRepository, bus);

        // Create an instance of CLIAutocompleteService
        CLIAutocompleteService autocompleteService(ioService, commandService, command, listService, listItemService);

        // Test getCompletion function
        REQUIRE_NOTHROW(autocompleteService.getCompletion());

        // Capture the printed output of getCompletion()
        std::string printedString = captureGetCompletionOutput(autocompleteService);
        std::string expectedString = "";
        REQUIRE(printedString == expectedString);
    }

    SECTION("Autocomplete commands: commands list remove")
    {
        Command command = Command("commands", { "list", "remove" }, {}, "commands list remove");

        ConfigRepository configRepository(fileDataConfigStorageServicePtr.get(), init.getConfigFilePath());
        ConfigRepository cacheRepository(fileDataConfigStorageServicePtr.get(), init.getCacheFilePath());
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService = PriorityService();
        StatusService statusService = StatusService();
        ListItemRepository listItemRepository(
            configService, fileDataStorageServicePtr.get(), priorityService, statusService);
        ListItemService listItemService(
            ioService, configService, listItemRepository, priorityService, statusService, bus);
        ListRepository listRepository(configService, fileDataStorageServicePtr.get());
        ListService listService(ioService, configService, listRepository, bus);

        // Create an instance of CLIAutocompleteService
        CLIAutocompleteService autocompleteService(ioService, commandService, command, listService, listItemService);

        // Test getCompletion function
        REQUIRE_NOTHROW(autocompleteService.getCompletion());

        // Capture the printed output of getCompletion()
        std::string printedString = captureGetCompletionOutput(autocompleteService);
        std::string expectedString = "tempList2Name tempListName\n";
        REQUIRE(printedString == expectedString);
    }

    SECTION("Autocomplete commands: commands list copy")
    {
        Command command = Command("commands", { "list", "copy" }, {}, "commands list copy");

        ConfigRepository configRepository(fileDataConfigStorageServicePtr.get(), init.getConfigFilePath());
        ConfigRepository cacheRepository(fileDataConfigStorageServicePtr.get(), init.getCacheFilePath());
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService = PriorityService();
        StatusService statusService = StatusService();
        ListItemRepository listItemRepository(
            configService, fileDataStorageServicePtr.get(), priorityService, statusService);
        ListItemService listItemService(
            ioService, configService, listItemRepository, priorityService, statusService, bus);
        ListRepository listRepository(configService, fileDataStorageServicePtr.get());
        ListService listService(ioService, configService, listRepository, bus);

        // Create an instance of CLIAutocompleteService
        CLIAutocompleteService autocompleteService(ioService, commandService, command, listService, listItemService);

        // Test getCompletion function
        REQUIRE_NOTHROW(autocompleteService.getCompletion());

        // Capture the printed output of getCompletion()
        std::string printedString = captureGetCompletionOutput(autocompleteService);
        std::string expectedString = "tempList2Name tempListName\n";
        REQUIRE(printedString == expectedString);
    }

    SECTION("Autocomplete commands: commands list show")
    {
        Command command = Command("commands", { "list", "show" }, {}, "commands list show");

        ConfigRepository configRepository(fileDataConfigStorageServicePtr.get(), init.getConfigFilePath());
        ConfigRepository cacheRepository(fileDataConfigStorageServicePtr.get(), init.getCacheFilePath());
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService = PriorityService();
        StatusService statusService = StatusService();
        ListItemRepository listItemRepository(
            configService, fileDataStorageServicePtr.get(), priorityService, statusService);
        ListItemService listItemService(
            ioService, configService, listItemRepository, priorityService, statusService, bus);
        ListRepository listRepository(configService, fileDataStorageServicePtr.get());
        ListService listService(ioService, configService, listRepository, bus);

        // Create an instance of CLIAutocompleteService
        CLIAutocompleteService autocompleteService(ioService, commandService, command, listService, listItemService);

        // Test getCompletion function
        REQUIRE_NOTHROW(autocompleteService.getCompletion());

        // Capture the printed output of getCompletion()
        std::string printedString = captureGetCompletionOutput(autocompleteService);
        std::string expectedString = "";
        REQUIRE(printedString == expectedString);
    }

    SECTION("Autocomplete commands: commands use")
    {
        Command command = Command("commands", { "use" }, {}, "commands use");

        ConfigRepository configRepository(fileDataConfigStorageServicePtr.get(), init.getConfigFilePath());
        ConfigRepository cacheRepository(fileDataConfigStorageServicePtr.get(), init.getCacheFilePath());
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService = PriorityService();
        StatusService statusService = StatusService();
        ListItemRepository listItemRepository(
            configService, fileDataStorageServicePtr.get(), priorityService, statusService);
        ListItemService listItemService(
            ioService, configService, listItemRepository, priorityService, statusService, bus);
        ListRepository listRepository(configService, fileDataStorageServicePtr.get());
        ListService listService(ioService, configService, listRepository, bus);

        // Create an instance of CLIAutocompleteService
        CLIAutocompleteService autocompleteService(ioService, commandService, command, listService, listItemService);

        // Test getCompletion function
        REQUIRE_NOTHROW(autocompleteService.getCompletion());

        // Capture the printed output of getCompletion()
        std::string printedString = captureGetCompletionOutput(autocompleteService);
        std::string expectedString = "tempList2Name tempListName\n";
        REQUIRE(printedString == expectedString);
    }

    SECTION("Autocomplete commands: commands use tempListName")
    {
        Command command = Command("commands", { "use", "tempListName" }, {}, "commands use tempListName");

        ConfigRepository configRepository(fileDataConfigStorageServicePtr.get(), init.getConfigFilePath());
        ConfigRepository cacheRepository(fileDataConfigStorageServicePtr.get(), init.getCacheFilePath());
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService = PriorityService();
        StatusService statusService = StatusService();
        ListItemRepository listItemRepository(
            configService, fileDataStorageServicePtr.get(), priorityService, statusService);
        ListItemService listItemService(
            ioService, configService, listItemRepository, priorityService, statusService, bus);
        ListRepository listRepository(configService, fileDataStorageServicePtr.get());
        ListService listService(ioService, configService, listRepository, bus);

        // Create an instance of CLIAutocompleteService
        CLIAutocompleteService autocompleteService(ioService, commandService, command, listService, listItemService);

        // Test getCompletion function
        REQUIRE_NOTHROW(autocompleteService.getCompletion());

        // Capture the printed output of getCompletion()
        std::string printedString = captureGetCompletionOutput(autocompleteService);
        std::string expectedString = "";
        REQUIRE(printedString == expectedString);
    }

    SECTION("Autocomplete commands: commands remove")
    {
        Command command = Command("commands", { "remove" }, {}, "commands remove");

        ConfigRepository configRepository(fileDataConfigStorageServicePtr.get(), init.getConfigFilePath());
        ConfigRepository cacheRepository(fileDataConfigStorageServicePtr.get(), init.getCacheFilePath());
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService = PriorityService();
        StatusService statusService = StatusService();
        ListItemRepository listItemRepository(
            configService, fileDataStorageServicePtr.get(), priorityService, statusService);
        ListItemService listItemService(
            ioService, configService, listItemRepository, priorityService, statusService, bus);
        ListRepository listRepository(configService, fileDataStorageServicePtr.get());
        ListService listService(ioService, configService, listRepository, bus);

        // Create an instance of CLIAutocompleteService
        CLIAutocompleteService autocompleteService(ioService, commandService, command, listService, listItemService);

        // Test getCompletion function
        REQUIRE_NOTHROW(autocompleteService.getCompletion());

        // Capture the printed output of getCompletion()
        std::string printedString = captureGetCompletionOutput(autocompleteService);
        std::string expectedString = "aaaa bbbb\n";
        REQUIRE(printedString == expectedString);
    }

    SECTION("Autocomplete commands: commands remove aaaa")
    {
        Command command = Command("commands", { "remove", "aaaa" }, {}, "commands remove aaaa");

        ConfigRepository configRepository(fileDataConfigStorageServicePtr.get(), init.getConfigFilePath());
        ConfigRepository cacheRepository(fileDataConfigStorageServicePtr.get(), init.getCacheFilePath());
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService = PriorityService();
        StatusService statusService = StatusService();
        ListItemRepository listItemRepository(
            configService, fileDataStorageServicePtr.get(), priorityService, statusService);
        ListItemService listItemService(
            ioService, configService, listItemRepository, priorityService, statusService, bus);
        ListRepository listRepository(configService, fileDataStorageServicePtr.get());
        ListService listService(ioService, configService, listRepository, bus);

        // Create an instance of CLIAutocompleteService
        CLIAutocompleteService autocompleteService(ioService, commandService, command, listService, listItemService);

        // Test getCompletion function
        REQUIRE_NOTHROW(autocompleteService.getCompletion());

        // Capture the printed output of getCompletion()
        std::string printedString = captureGetCompletionOutput(autocompleteService);
        std::string expectedString = "aaaa bbbb\n";
        REQUIRE(printedString == expectedString);
    }

    SECTION("Autocomplete commands: commands reset")
    {
        Command command = Command("commands", { "reset" }, {}, "commands reset");

        ConfigRepository configRepository(fileDataConfigStorageServicePtr.get(), init.getConfigFilePath());
        ConfigRepository cacheRepository(fileDataConfigStorageServicePtr.get(), init.getCacheFilePath());
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService = PriorityService();
        StatusService statusService = StatusService();
        ListItemRepository listItemRepository(
            configService, fileDataStorageServicePtr.get(), priorityService, statusService);
        ListItemService listItemService(
            ioService, configService, listItemRepository, priorityService, statusService, bus);
        ListRepository listRepository(configService, fileDataStorageServicePtr.get());
        ListService listService(ioService, configService, listRepository, bus);

        // Create an instance of CLIAutocompleteService
        CLIAutocompleteService autocompleteService(ioService, commandService, command, listService, listItemService);

        // Test getCompletion function
        REQUIRE_NOTHROW(autocompleteService.getCompletion());

        // Capture the printed output of getCompletion()
        std::string printedString = captureGetCompletionOutput(autocompleteService);
        std::string expectedString = "aaaa bbbb\n";
        REQUIRE(printedString == expectedString);
    }

    SECTION("Autocomplete commands: commands reset aaaa")
    {
        Command command = Command("commands", { "reset", "aaaa" }, {}, "commands reset aaaa");

        ConfigRepository configRepository(fileDataConfigStorageServicePtr.get(), init.getConfigFilePath());
        ConfigRepository cacheRepository(fileDataConfigStorageServicePtr.get(), init.getCacheFilePath());
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService = PriorityService();
        StatusService statusService = StatusService();
        ListItemRepository listItemRepository(
            configService, fileDataStorageServicePtr.get(), priorityService, statusService);
        ListItemService listItemService(
            ioService, configService, listItemRepository, priorityService, statusService, bus);
        ListRepository listRepository(configService, fileDataStorageServicePtr.get());
        ListService listService(ioService, configService, listRepository, bus);

        // Create an instance of CLIAutocompleteService
        CLIAutocompleteService autocompleteService(ioService, commandService, command, listService, listItemService);

        // Test getCompletion function
        REQUIRE_NOTHROW(autocompleteService.getCompletion());

        // Capture the printed output of getCompletion()
        std::string printedString = captureGetCompletionOutput(autocompleteService);
        std::string expectedString = "aaaa bbbb\n";
        REQUIRE(printedString == expectedString);
    }

    SECTION("Autocomplete commands: commands pause")
    {
        Command command = Command("commands", { "pause" }, {}, "commands pause");

        ConfigRepository configRepository(fileDataConfigStorageServicePtr.get(), init.getConfigFilePath());
        ConfigRepository cacheRepository(fileDataConfigStorageServicePtr.get(), init.getCacheFilePath());
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService = PriorityService();
        StatusService statusService = StatusService();
        ListItemRepository listItemRepository(
            configService, fileDataStorageServicePtr.get(), priorityService, statusService);
        ListItemService listItemService(
            ioService, configService, listItemRepository, priorityService, statusService, bus);
        ListRepository listRepository(configService, fileDataStorageServicePtr.get());
        ListService listService(ioService, configService, listRepository, bus);

        // Create an instance of CLIAutocompleteService
        CLIAutocompleteService autocompleteService(ioService, commandService, command, listService, listItemService);

        // Test getCompletion function
        REQUIRE_NOTHROW(autocompleteService.getCompletion());

        // Capture the printed output of getCompletion()
        std::string printedString = captureGetCompletionOutput(autocompleteService);
        std::string expectedString = "aaaa bbbb\n";
        REQUIRE(printedString == expectedString);
    }

    SECTION("Autocomplete commands: commands pause aaaa")
    {
        Command command = Command("commands", { "pause", "aaaa" }, {}, "commands pause aaaa");

        ConfigRepository configRepository(fileDataConfigStorageServicePtr.get(), init.getConfigFilePath());
        ConfigRepository cacheRepository(fileDataConfigStorageServicePtr.get(), init.getCacheFilePath());
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService = PriorityService();
        StatusService statusService = StatusService();
        ListItemRepository listItemRepository(
            configService, fileDataStorageServicePtr.get(), priorityService, statusService);
        ListItemService listItemService(
            ioService, configService, listItemRepository, priorityService, statusService, bus);
        ListRepository listRepository(configService, fileDataStorageServicePtr.get());
        ListService listService(ioService, configService, listRepository, bus);

        // Create an instance of CLIAutocompleteService
        CLIAutocompleteService autocompleteService(ioService, commandService, command, listService, listItemService);

        // Test getCompletion function
        REQUIRE_NOTHROW(autocompleteService.getCompletion());

        // Capture the printed output of getCompletion()
        std::string printedString = captureGetCompletionOutput(autocompleteService);
        std::string expectedString = "aaaa bbbb\n";
        REQUIRE(printedString == expectedString);
    }

    SECTION("Autocomplete commands: commands complete")
    {
        Command command = Command("commands", { "complete" }, {}, "commands complete");

        ConfigRepository configRepository(fileDataConfigStorageServicePtr.get(), init.getConfigFilePath());
        ConfigRepository cacheRepository(fileDataConfigStorageServicePtr.get(), init.getCacheFilePath());
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService = PriorityService();
        StatusService statusService = StatusService();
        ListItemRepository listItemRepository(
            configService, fileDataStorageServicePtr.get(), priorityService, statusService);
        ListItemService listItemService(
            ioService, configService, listItemRepository, priorityService, statusService, bus);
        ListRepository listRepository(configService, fileDataStorageServicePtr.get());
        ListService listService(ioService, configService, listRepository, bus);

        // Create an instance of CLIAutocompleteService
        CLIAutocompleteService autocompleteService(ioService, commandService, command, listService, listItemService);

        // Test getCompletion function
        REQUIRE_NOTHROW(autocompleteService.getCompletion());

        // Capture the printed output of getCompletion()
        std::string printedString = captureGetCompletionOutput(autocompleteService);
        std::string expectedString = "aaaa bbbb\n";
        REQUIRE(printedString == expectedString);
    }

    SECTION("Autocomplete commands: commands complete aaaa")
    {
        Command command = Command("commands", { "complete", "aaaa" }, {}, "commands complete aaaa");

        ConfigRepository configRepository(fileDataConfigStorageServicePtr.get(), init.getConfigFilePath());
        ConfigRepository cacheRepository(fileDataConfigStorageServicePtr.get(), init.getCacheFilePath());
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService = PriorityService();
        StatusService statusService = StatusService();
        ListItemRepository listItemRepository(
            configService, fileDataStorageServicePtr.get(), priorityService, statusService);
        ListItemService listItemService(
            ioService, configService, listItemRepository, priorityService, statusService, bus);
        ListRepository listRepository(configService, fileDataStorageServicePtr.get());
        ListService listService(ioService, configService, listRepository, bus);

        // Create an instance of CLIAutocompleteService
        CLIAutocompleteService autocompleteService(ioService, commandService, command, listService, listItemService);

        // Test getCompletion function
        REQUIRE_NOTHROW(autocompleteService.getCompletion());

        // Capture the printed output of getCompletion()
        std::string printedString = captureGetCompletionOutput(autocompleteService);
        std::string expectedString = "aaaa bbbb\n";
        REQUIRE(printedString == expectedString);
    }

    SECTION("Autocomplete commands: commands cancel")
    {
        Command command = Command("commands", { "cancel" }, {}, "commands cancel");

        ConfigRepository configRepository(fileDataConfigStorageServicePtr.get(), init.getConfigFilePath());
        ConfigRepository cacheRepository(fileDataConfigStorageServicePtr.get(), init.getCacheFilePath());
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService = PriorityService();
        StatusService statusService = StatusService();
        ListItemRepository listItemRepository(
            configService, fileDataStorageServicePtr.get(), priorityService, statusService);
        ListItemService listItemService(
            ioService, configService, listItemRepository, priorityService, statusService, bus);
        ListRepository listRepository(configService, fileDataStorageServicePtr.get());
        ListService listService(ioService, configService, listRepository, bus);

        // Create an instance of CLIAutocompleteService
        CLIAutocompleteService autocompleteService(ioService, commandService, command, listService, listItemService);

        // Test getCompletion function
        REQUIRE_NOTHROW(autocompleteService.getCompletion());

        // Capture the printed output of getCompletion()
        std::string printedString = captureGetCompletionOutput(autocompleteService);
        std::string expectedString = "aaaa bbbb\n";
        REQUIRE(printedString == expectedString);
    }

    SECTION("Autocomplete commands: commands cancel aaaa")
    {
        Command command = Command("commands", { "cancel", "aaaa" }, {}, "commands cancel aaaa");

        ConfigRepository configRepository(fileDataConfigStorageServicePtr.get(), init.getConfigFilePath());
        ConfigRepository cacheRepository(fileDataConfigStorageServicePtr.get(), init.getCacheFilePath());
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService = PriorityService();
        StatusService statusService = StatusService();
        ListItemRepository listItemRepository(
            configService, fileDataStorageServicePtr.get(), priorityService, statusService);
        ListItemService listItemService(
            ioService, configService, listItemRepository, priorityService, statusService, bus);
        ListRepository listRepository(configService, fileDataStorageServicePtr.get());
        ListService listService(ioService, configService, listRepository, bus);

        // Create an instance of CLIAutocompleteService
        CLIAutocompleteService autocompleteService(ioService, commandService, command, listService, listItemService);

        // Test getCompletion function
        REQUIRE_NOTHROW(autocompleteService.getCompletion());

        // Capture the printed output of getCompletion()
        std::string printedString = captureGetCompletionOutput(autocompleteService);
        std::string expectedString = "aaaa bbbb\n";
        REQUIRE(printedString == expectedString);
    }

    SECTION("Autocomplete commands: commands increase")
    {
        Command command = Command("commands", { "increase" }, {}, "commands increase");

        ConfigRepository configRepository(fileDataConfigStorageServicePtr.get(), init.getConfigFilePath());
        ConfigRepository cacheRepository(fileDataConfigStorageServicePtr.get(), init.getCacheFilePath());
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService = PriorityService();
        StatusService statusService = StatusService();
        ListItemRepository listItemRepository(
            configService, fileDataStorageServicePtr.get(), priorityService, statusService);
        ListItemService listItemService(
            ioService, configService, listItemRepository, priorityService, statusService, bus);
        ListRepository listRepository(configService, fileDataStorageServicePtr.get());
        ListService listService(ioService, configService, listRepository, bus);

        // Create an instance of CLIAutocompleteService
        CLIAutocompleteService autocompleteService(ioService, commandService, command, listService, listItemService);

        // Test getCompletion function
        REQUIRE_NOTHROW(autocompleteService.getCompletion());

        // Capture the printed output of getCompletion()
        std::string printedString = captureGetCompletionOutput(autocompleteService);
        std::string expectedString = "aaaa bbbb\n";
        REQUIRE(printedString == expectedString);
    }

    SECTION("Autocomplete commands: commands increase aaaa")
    {
        Command command = Command("commands", { "increase", "aaaa" }, {}, "commands increase aaaa");

        ConfigRepository configRepository(fileDataConfigStorageServicePtr.get(), init.getConfigFilePath());
        ConfigRepository cacheRepository(fileDataConfigStorageServicePtr.get(), init.getCacheFilePath());
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService = PriorityService();
        StatusService statusService = StatusService();
        ListItemRepository listItemRepository(
            configService, fileDataStorageServicePtr.get(), priorityService, statusService);
        ListItemService listItemService(
            ioService, configService, listItemRepository, priorityService, statusService, bus);
        ListRepository listRepository(configService, fileDataStorageServicePtr.get());
        ListService listService(ioService, configService, listRepository, bus);

        // Create an instance of CLIAutocompleteService
        CLIAutocompleteService autocompleteService(ioService, commandService, command, listService, listItemService);

        // Test getCompletion function
        REQUIRE_NOTHROW(autocompleteService.getCompletion());

        // Capture the printed output of getCompletion()
        std::string printedString = captureGetCompletionOutput(autocompleteService);
        std::string expectedString = "aaaa bbbb\n";
        REQUIRE(printedString == expectedString);
    }

    SECTION("Autocomplete commands: commands decrease")
    {
        Command command = Command("commands", { "decrease" }, {}, "commands decrease");

        ConfigRepository configRepository(fileDataConfigStorageServicePtr.get(), init.getConfigFilePath());
        ConfigRepository cacheRepository(fileDataConfigStorageServicePtr.get(), init.getCacheFilePath());
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService = PriorityService();
        StatusService statusService = StatusService();
        ListItemRepository listItemRepository(
            configService, fileDataStorageServicePtr.get(), priorityService, statusService);
        ListItemService listItemService(
            ioService, configService, listItemRepository, priorityService, statusService, bus);
        ListRepository listRepository(configService, fileDataStorageServicePtr.get());
        ListService listService(ioService, configService, listRepository, bus);

        // Create an instance of CLIAutocompleteService
        CLIAutocompleteService autocompleteService(ioService, commandService, command, listService, listItemService);

        // Test getCompletion function
        REQUIRE_NOTHROW(autocompleteService.getCompletion());

        // Capture the printed output of getCompletion()
        std::string printedString = captureGetCompletionOutput(autocompleteService);
        std::string expectedString = "aaaa bbbb\n";
        REQUIRE(printedString == expectedString);
    }

    SECTION("Autocomplete commands: commands decrease aaaa")
    {
        Command command = Command("commands", { "decrease", "aaaa" }, {}, "commands decrease aaaa");

        ConfigRepository configRepository(fileDataConfigStorageServicePtr.get(), init.getConfigFilePath());
        ConfigRepository cacheRepository(fileDataConfigStorageServicePtr.get(), init.getCacheFilePath());
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService = PriorityService();
        StatusService statusService = StatusService();
        ListItemRepository listItemRepository(
            configService, fileDataStorageServicePtr.get(), priorityService, statusService);
        ListItemService listItemService(
            ioService, configService, listItemRepository, priorityService, statusService, bus);
        ListRepository listRepository(configService, fileDataStorageServicePtr.get());
        ListService listService(ioService, configService, listRepository, bus);

        // Create an instance of CLIAutocompleteService
        CLIAutocompleteService autocompleteService(ioService, commandService, command, listService, listItemService);

        // Test getCompletion function
        REQUIRE_NOTHROW(autocompleteService.getCompletion());

        // Capture the printed output of getCompletion()
        std::string printedString = captureGetCompletionOutput(autocompleteService);
        std::string expectedString = "aaaa bbbb\n";
        REQUIRE(printedString == expectedString);
    }

    SECTION("Autocomplete commands: commands append")
    {
        Command command = Command("commands", { "append" }, {}, "commands append");

        ConfigRepository configRepository(fileDataConfigStorageServicePtr.get(), init.getConfigFilePath());
        ConfigRepository cacheRepository(fileDataConfigStorageServicePtr.get(), init.getCacheFilePath());
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService = PriorityService();
        StatusService statusService = StatusService();
        ListItemRepository listItemRepository(
            configService, fileDataStorageServicePtr.get(), priorityService, statusService);
        ListItemService listItemService(
            ioService, configService, listItemRepository, priorityService, statusService, bus);
        ListRepository listRepository(configService, fileDataStorageServicePtr.get());
        ListService listService(ioService, configService, listRepository, bus);

        // Create an instance of CLIAutocompleteService
        CLIAutocompleteService autocompleteService(ioService, commandService, command, listService, listItemService);

        // Test getCompletion function
        REQUIRE_NOTHROW(autocompleteService.getCompletion());

        // Capture the printed output of getCompletion()
        std::string printedString = captureGetCompletionOutput(autocompleteService);
        std::string expectedString = "aaaa bbbb\n";
        REQUIRE(printedString == expectedString);
    }

    SECTION("Autocomplete commands: commands append aaaa")
    {
        Command command = Command("commands", { "append", "aaaa" }, {}, "commands append aaaa");

        ConfigRepository configRepository(fileDataConfigStorageServicePtr.get(), init.getConfigFilePath());
        ConfigRepository cacheRepository(fileDataConfigStorageServicePtr.get(), init.getCacheFilePath());
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService = PriorityService();
        StatusService statusService = StatusService();
        ListItemRepository listItemRepository(
            configService, fileDataStorageServicePtr.get(), priorityService, statusService);
        ListItemService listItemService(
            ioService, configService, listItemRepository, priorityService, statusService, bus);
        ListRepository listRepository(configService, fileDataStorageServicePtr.get());
        ListService listService(ioService, configService, listRepository, bus);

        // Create an instance of CLIAutocompleteService
        CLIAutocompleteService autocompleteService(ioService, commandService, command, listService, listItemService);

        // Test getCompletion function
        REQUIRE_NOTHROW(autocompleteService.getCompletion());

        // Capture the printed output of getCompletion()
        std::string printedString = captureGetCompletionOutput(autocompleteService);
        std::string expectedString = "";
        REQUIRE(printedString == expectedString);
    }

    SECTION("Autocomplete commands: commands prepend")
    {
        Command command = Command("commands", { "prepend" }, {}, "commands prepend");

        ConfigRepository configRepository(fileDataConfigStorageServicePtr.get(), init.getConfigFilePath());
        ConfigRepository cacheRepository(fileDataConfigStorageServicePtr.get(), init.getCacheFilePath());
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService = PriorityService();
        StatusService statusService = StatusService();
        ListItemRepository listItemRepository(
            configService, fileDataStorageServicePtr.get(), priorityService, statusService);
        ListItemService listItemService(
            ioService, configService, listItemRepository, priorityService, statusService, bus);
        ListRepository listRepository(configService, fileDataStorageServicePtr.get());
        ListService listService(ioService, configService, listRepository, bus);

        // Create an instance of CLIAutocompleteService
        CLIAutocompleteService autocompleteService(ioService, commandService, command, listService, listItemService);

        // Test getCompletion function
        REQUIRE_NOTHROW(autocompleteService.getCompletion());

        // Capture the printed output of getCompletion()
        std::string printedString = captureGetCompletionOutput(autocompleteService);
        std::string expectedString = "aaaa bbbb\n";
        REQUIRE(printedString == expectedString);
    }

    SECTION("Autocomplete commands: commands prepend aaaa")
    {
        Command command = Command("commands", { "prepend", "aaaa" }, {}, "commands prepend aaaa");

        ConfigRepository configRepository(fileDataConfigStorageServicePtr.get(), init.getConfigFilePath());
        ConfigRepository cacheRepository(fileDataConfigStorageServicePtr.get(), init.getCacheFilePath());
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService = PriorityService();
        StatusService statusService = StatusService();
        ListItemRepository listItemRepository(
            configService, fileDataStorageServicePtr.get(), priorityService, statusService);
        ListItemService listItemService(
            ioService, configService, listItemRepository, priorityService, statusService, bus);
        ListRepository listRepository(configService, fileDataStorageServicePtr.get());
        ListService listService(ioService, configService, listRepository, bus);

        // Create an instance of CLIAutocompleteService
        CLIAutocompleteService autocompleteService(ioService, commandService, command, listService, listItemService);

        // Test getCompletion function
        REQUIRE_NOTHROW(autocompleteService.getCompletion());

        // Capture the printed output of getCompletion()
        std::string printedString = captureGetCompletionOutput(autocompleteService);
        std::string expectedString = "";
        REQUIRE(printedString == expectedString);
    }

    SECTION("Autocomplete commands: commands deadline")
    {
        Command command = Command("commands", { "deadline", "today" }, {}, "commands deadline today");

        ConfigRepository configRepository(fileDataConfigStorageServicePtr.get(), init.getConfigFilePath());
        ConfigRepository cacheRepository(fileDataConfigStorageServicePtr.get(), init.getCacheFilePath());
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService = PriorityService();
        StatusService statusService = StatusService();
        ListItemRepository listItemRepository(
            configService, fileDataStorageServicePtr.get(), priorityService, statusService);
        ListItemService listItemService(
            ioService, configService, listItemRepository, priorityService, statusService, bus);
        ListRepository listRepository(configService, fileDataStorageServicePtr.get());
        ListService listService(ioService, configService, listRepository, bus);

        // Create an instance of CLIAutocompleteService
        CLIAutocompleteService autocompleteService(ioService, commandService, command, listService, listItemService);

        // Test getCompletion function
        REQUIRE_NOTHROW(autocompleteService.getCompletion());

        // Capture the printed output of getCompletion()
        std::string printedString = captureGetCompletionOutput(autocompleteService);
        std::string expectedString = "aaaa bbbb\n";
        REQUIRE(printedString == expectedString);
    }

    SECTION("Autocomplete commands: commands deadline aaaa")
    {
        Command command = Command("commands", { "deadline" }, {}, "commands deadline");

        ConfigRepository configRepository(fileDataConfigStorageServicePtr.get(), init.getConfigFilePath());
        ConfigRepository cacheRepository(fileDataConfigStorageServicePtr.get(), init.getCacheFilePath());
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService = PriorityService();
        StatusService statusService = StatusService();
        ListItemRepository listItemRepository(
            configService, fileDataStorageServicePtr.get(), priorityService, statusService);
        ListItemService listItemService(
            ioService, configService, listItemRepository, priorityService, statusService, bus);
        ListRepository listRepository(configService, fileDataStorageServicePtr.get());
        ListService listService(ioService, configService, listRepository, bus);

        // Create an instance of CLIAutocompleteService
        CLIAutocompleteService autocompleteService(ioService, commandService, command, listService, listItemService);

        // Test getCompletion function
        REQUIRE_NOTHROW(autocompleteService.getCompletion());

        // Capture the printed output of getCompletion()
        std::string printedString = captureGetCompletionOutput(autocompleteService);
        std::string expectedString = "today tomorrow monday tuesday wednesday thursday friday saturday sunday "
                                     "next-week next-month next-year reset\n";
        REQUIRE(printedString == expectedString);
    }

    SECTION("Autocomplete commands: commands move-to")
    {
        Command command = Command("commands", { "move-to" }, {}, "commands move-to");

        ConfigRepository configRepository(fileDataConfigStorageServicePtr.get(), init.getConfigFilePath());
        ConfigRepository cacheRepository(fileDataConfigStorageServicePtr.get(), init.getCacheFilePath());
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService = PriorityService();
        StatusService statusService = StatusService();
        ListItemRepository listItemRepository(
            configService, fileDataStorageServicePtr.get(), priorityService, statusService);
        ListItemService listItemService(
            ioService, configService, listItemRepository, priorityService, statusService, bus);
        ListRepository listRepository(configService, fileDataStorageServicePtr.get());
        ListService listService(ioService, configService, listRepository, bus);

        // Create an instance of CLIAutocompleteService
        CLIAutocompleteService autocompleteService(ioService, commandService, command, listService, listItemService);

        // Test getCompletion function
        REQUIRE_NOTHROW(autocompleteService.getCompletion());

        // Capture the printed output of getCompletion()
        std::string printedString = captureGetCompletionOutput(autocompleteService);
        std::string expectedString = "tempList2Name tempListName\n";
        REQUIRE(printedString == expectedString);
    }

    SECTION("Autocomplete commands: commands move tempListName")
    {
        Command command = Command("commands", { "move", "tempListName" }, {}, "commands move tempListName");

        ConfigRepository configRepository(fileDataConfigStorageServicePtr.get(), init.getConfigFilePath());
        ConfigRepository cacheRepository(fileDataConfigStorageServicePtr.get(), init.getCacheFilePath());
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService = PriorityService();
        StatusService statusService = StatusService();
        ListItemRepository listItemRepository(
            configService, fileDataStorageServicePtr.get(), priorityService, statusService);
        ListItemService listItemService(
            ioService, configService, listItemRepository, priorityService, statusService, bus);
        ListRepository listRepository(configService, fileDataStorageServicePtr.get());
        ListService listService(ioService, configService, listRepository, bus);

        // Create an instance of CLIAutocompleteService
        CLIAutocompleteService autocompleteService(ioService, commandService, command, listService, listItemService);

        // Test getCompletion function
        REQUIRE_NOTHROW(autocompleteService.getCompletion());

        // Capture the printed output of getCompletion()
        std::string printedString = captureGetCompletionOutput(autocompleteService);
        std::string expectedString = "aaaa bbbb\n";
        REQUIRE(printedString == expectedString);
    }

    SECTION("Autocomplete commands: commands move tempList2Name aaaa")
    {
        Command command =
            Command("commands", { "move", "tempList2Name", "aaaa" }, {}, "commands move tempList2Name aaaa");

        ConfigRepository configRepository(fileDataConfigStorageServicePtr.get(), init.getConfigFilePath());
        ConfigRepository cacheRepository(fileDataConfigStorageServicePtr.get(), init.getCacheFilePath());
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService = PriorityService();
        StatusService statusService = StatusService();
        ListItemRepository listItemRepository(
            configService, fileDataStorageServicePtr.get(), priorityService, statusService);
        ListItemService listItemService(
            ioService, configService, listItemRepository, priorityService, statusService, bus);
        ListRepository listRepository(configService, fileDataStorageServicePtr.get());
        ListService listService(ioService, configService, listRepository, bus);

        // Create an instance of CLIAutocompleteService
        CLIAutocompleteService autocompleteService(ioService, commandService, command, listService, listItemService);

        // Test getCompletion function
        REQUIRE_NOTHROW(autocompleteService.getCompletion());

        // Capture the printed output of getCompletion()
        std::string printedString = captureGetCompletionOutput(autocompleteService);
        std::string expectedString = "aaaa bbbb\n";
        REQUIRE(printedString == expectedString);
    }

    SECTION("Autocomplete commands: commands move aaaa tempList2")
    {
        Command command = Command("commands", { "move", "aaaa", "tempList2" }, {}, "commands move aaaa tempList2");

        ConfigRepository configRepository(fileDataConfigStorageServicePtr.get(), init.getConfigFilePath());
        ConfigRepository cacheRepository(fileDataConfigStorageServicePtr.get(), init.getCacheFilePath());
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService = PriorityService();
        StatusService statusService = StatusService();
        ListItemRepository listItemRepository(
            configService, fileDataStorageServicePtr.get(), priorityService, statusService);
        ListItemService listItemService(
            ioService, configService, listItemRepository, priorityService, statusService, bus);
        ListRepository listRepository(configService, fileDataStorageServicePtr.get());
        ListService listService(ioService, configService, listRepository, bus);

        // Create an instance of CLIAutocompleteService
        CLIAutocompleteService autocompleteService(ioService, commandService, command, listService, listItemService);

        // Test getCompletion function
        REQUIRE_NOTHROW(autocompleteService.getCompletion());

        // Capture the printed output of getCompletion()
        std::string printedString = captureGetCompletionOutput(autocompleteService);
        std::string expectedString = "tempList2Name tempListName\n";
        REQUIRE(printedString == expectedString);
    }

    SECTION("Autocomplete commands: commands archive")
    {

        installation.wipe();
        installation.make();

        Command command = Command("commands", { "archive" }, {}, "commands archive");

        ConfigRepository configRepository(fileDataConfigStorageServicePtr.get(), init.getConfigFilePath());
        ConfigRepository cacheRepository(fileDataConfigStorageServicePtr.get(), init.getCacheFilePath());
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService = PriorityService();
        StatusService statusService = StatusService();
        ListItemRepository listItemRepository(
            configService, fileDataStorageServicePtr.get(), priorityService, statusService);
        ListItemService listItemService(
            ioService, configService, listItemRepository, priorityService, statusService, bus);
        ListRepository listRepository(configService, fileDataStorageServicePtr.get());
        ListService listService(ioService, configService, listRepository, bus);

        // Create an instance of CLIAutocompleteService
        CLIAutocompleteService autocompleteService(ioService, commandService, command, listService, listItemService);

        // Test getCompletion function
        REQUIRE_NOTHROW(autocompleteService.getCompletion());

        // Capture the printed output of getCompletion()
        std::string printedString = captureGetCompletionOutput(autocompleteService);
        std::string expectedString = "aaaa bbbb\n";
        REQUIRE(printedString == expectedString);
    }
}
