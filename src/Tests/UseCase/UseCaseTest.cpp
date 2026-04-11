#include "../../Command/CommandList.h"
#include "../../Command/CommandOption.h"
#include "../../Command/CommandService.h"
#include "../../Events/EventBus.h"
#include "../../FileDataStorage/ConfService.h"
#include "../../FileDataStorage/JSONService.h"
#include "../../FileDataStorageRepositories/ListItemRepository.h"
#include "../../FileDataStorageRepositories/ListRepository.h"
#include "../../FileStorage/FileStorageService.h"
#include "../../Help/Help.h"
#include "../../Themes/ThemeService.h"
#include "../../UseCase/ArchiveUseCase.h"
#include "../../UseCase/CleanUseCase.h"
#include "../../UseCase/EmptyUseCase.h"
#include "../../UseCase/FindUseCase.h"
#include "../../UseCase/ListItemActionsUseCase.h"
#include "../../UseCase/ListUseCase.h"
#include "../../UseCase/MoveUseCase.h"
#include "../../UseCase/PriorityUseCase.h"
#include "../../UseCase/RemoveUseCase.h"
#include "../../UseCase/ResetUseCase.h"
#include "../../UseCase/RestoreUseCase.h"
#include "../../UseCase/ShowUseCase.h"
#include "../../UseCase/StatsUseCase.h"
#include "../../UseCase/StatusUseCase.h"
#include "../../UseCase/SwitchListUseCase.h"
#include "../Mock/MockInit.h"
#include "../Mock/MockInstallation.h"
#include <catch2/catch_test_macros.hpp>

// ---------------------------------------------------------------------------
// ShowUseCase
// ---------------------------------------------------------------------------

TEST_CASE("ShowUseCase", "[UseCase][Show]")
{
    IOService ioService("cli");
    ConfService confService(ioService);
    JSONService jsonService(ioService);
    std::unique_ptr<FileDataServiceInterface> storagePtr = std::make_unique<JSONService>(ioService);
    std::unique_ptr<FileDataServiceInterface> configStoragePtr = std::make_unique<ConfService>(ioService);
    MockInit init(ioService, "_todoos_UseCaseShow");
    MockInstallation installation(ioService, jsonService, confService, init);
    installation.wipe();
    installation.make();

    EventBus bus;
    CommandList commandList;
    CommandOption commandOption;
    CommandService commandService(commandList, commandOption);
    Help help(ioService);
    Command command("show", {}, {}, "show");
    ConfigRepository configRepository(configStoragePtr.get(), init.getConfigFilePath());
    ConfigRepository cacheRepository(configStoragePtr.get(), init.getCacheFilePath());
    ConfigService configService(ioService, init, configRepository, cacheRepository, command);
    PriorityService priorityService;
    StatusService statusService;
    ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
    ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
    ListRepository listRepository(configService, storagePtr.get());
    ListService listService(ioService, configService, listRepository, bus);
    ThemeService themeService(ioService, configService, listService, listItemService);

    SECTION("execute with default command does not throw")
    {
        REQUIRE_NOTHROW(
            ShowUseCase(
                ioService, help, commandService, command, configService, listService, listItemService, themeService)
                .execute());
    }

    SECTION("execute with named list does not throw")
    {
        Command namedCmd("show", { "tempListName" }, {}, "show tempListName");
        ConfigService cs(ioService, init, configRepository, cacheRepository, namedCmd);
        ListItemRepository lir(cs, storagePtr.get(), priorityService, statusService);
        ListItemService lis(ioService, cs, lir, priorityService, statusService);
        ListRepository lr(cs, storagePtr.get());
        ListService ls(ioService, cs, lr, bus);
        ThemeService theme(ioService, cs, ls, lis);
        REQUIRE_NOTHROW(ShowUseCase(ioService, help, commandService, namedCmd, cs, ls, lis, theme).execute());
    }
}

// ---------------------------------------------------------------------------
// ListItemActionsUseCase
// ---------------------------------------------------------------------------

TEST_CASE("ListItemActionsUseCase", "[UseCase][ListItemActions]")
{
    IOService ioService("cli");
    ConfService confService(ioService);
    JSONService jsonService(ioService);
    std::unique_ptr<FileDataServiceInterface> storagePtr = std::make_unique<JSONService>(ioService);
    std::unique_ptr<FileDataServiceInterface> configStoragePtr = std::make_unique<ConfService>(ioService);
    MockInit init(ioService, "_todoos_UseCaseListItemActions");
    MockInstallation installation(ioService, jsonService, confService, init);
    installation.wipe();
    installation.make();

    EventBus bus;
    CommandList commandList;
    CommandOption commandOption;
    CommandService commandService(commandList, commandOption);

    SECTION("add command increases item count by one")
    {
        Command command("add", { "new", "test", "item" }, {}, "add new test item");
        ConfigRepository configRepository(configStoragePtr.get(), init.getConfigFilePath());
        ConfigRepository cacheRepository(configStoragePtr.get(), init.getCacheFilePath());
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService;
        StatusService statusService;
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        ListRepository listRepository(configService, storagePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ThemeService themeService(ioService, configService, listService, listItemService);
        ListName listName = listService.createUsedListName();

        REQUIRE_NOTHROW(
            ListItemActionsUseCase(
                ioService, command, commandService, listItemService, listService, configService, themeService)
                .execute());

        std::vector<ListItemEntity> items = listItemService.get(listName);
        REQUIRE(items.size() == 3);

        installation.wipe();
        installation.make();
    }

    SECTION("add with no arguments does not throw and does not add item")
    {
        Command command("add", {}, {}, "add");
        ConfigRepository configRepository(configStoragePtr.get(), init.getConfigFilePath());
        ConfigRepository cacheRepository(configStoragePtr.get(), init.getCacheFilePath());
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService;
        StatusService statusService;
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        ListRepository listRepository(configService, storagePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ThemeService themeService(ioService, configService, listService, listItemService);
        ListName listName = listService.createUsedListName();

        REQUIRE_NOTHROW(
            ListItemActionsUseCase(
                ioService, command, commandService, listItemService, listService, configService, themeService)
                .execute());

        std::vector<ListItemEntity> items = listItemService.get(listName);
        REQUIRE(items.size() == 2);
    }
}

// ---------------------------------------------------------------------------
// FindUseCase
// ---------------------------------------------------------------------------

TEST_CASE("FindUseCase", "[UseCase][Find]")
{
    IOService ioService("cli");
    ConfService confService(ioService);
    JSONService jsonService(ioService);
    std::unique_ptr<FileDataServiceInterface> storagePtr = std::make_unique<JSONService>(ioService);
    std::unique_ptr<FileDataServiceInterface> configStoragePtr = std::make_unique<ConfService>(ioService);
    MockInit init(ioService, "_todoos_UseCaseFind");
    MockInstallation installation(ioService, jsonService, confService, init);
    installation.wipe();
    installation.make();

    EventBus bus;

    SECTION("search for existing value does not throw")
    {
        Command command("find", { "test" }, {}, "find test");
        ConfigRepository configRepository(configStoragePtr.get(), init.getConfigFilePath());
        ConfigRepository cacheRepository(configStoragePtr.get(), init.getCacheFilePath());
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService;
        StatusService statusService;
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        ListRepository listRepository(configService, storagePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ThemeService themeService(ioService, configService, listService, listItemService);

        REQUIRE_NOTHROW(
            FindUseCase(ioService, command, configService, listService, listItemService, themeService).execute());
    }

    SECTION("search for non-existing value does not throw")
    {
        Command command("find", { "zzznomatch" }, {}, "find zzznomatch");
        ConfigRepository configRepository(configStoragePtr.get(), init.getConfigFilePath());
        ConfigRepository cacheRepository(configStoragePtr.get(), init.getCacheFilePath());
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService;
        StatusService statusService;
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        ListRepository listRepository(configService, storagePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ThemeService themeService(ioService, configService, listService, listItemService);

        REQUIRE_NOTHROW(
            FindUseCase(ioService, command, configService, listService, listItemService, themeService).execute());
    }
}

// ---------------------------------------------------------------------------
// PriorityUseCase
// ---------------------------------------------------------------------------

TEST_CASE("PriorityUseCase", "[UseCase][Priority]")
{
    IOService ioService("cli");
    ConfService confService(ioService);
    JSONService jsonService(ioService);
    std::unique_ptr<FileDataServiceInterface> storagePtr = std::make_unique<JSONService>(ioService);
    std::unique_ptr<FileDataServiceInterface> configStoragePtr = std::make_unique<ConfService>(ioService);
    MockInit init(ioService, "_todoos_UseCasePriority");
    MockInstallation installation(ioService, jsonService, confService, init);
    installation.wipe();
    installation.make();

    EventBus bus;
    Command command("increase", { "aaaa" }, {}, "increase aaaa");
    ConfigRepository configRepository(configStoragePtr.get(), init.getConfigFilePath());
    ConfigRepository cacheRepository(configStoragePtr.get(), init.getCacheFilePath());
    ConfigService configService(ioService, init, configRepository, cacheRepository, command);
    PriorityService priorityService;
    StatusService statusService;
    ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
    ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
    ListRepository listRepository(configService, storagePtr.get());
    ListService listService(ioService, configService, listRepository, bus);
    ThemeService themeService(ioService, configService, listService, listItemService);
    ListName listName = listService.createUsedListName();

    SECTION("increase raises item priority")
    {
        REQUIRE_NOTHROW(
            PriorityUseCase(ioService, command, listItemService, listService, configService, themeService, "increase")
                .execute());

        ListItemEntity item = listItemService.find("aaaa", listName);
        REQUIRE(*(*item.priority()).getName() == "urgent");

        installation.wipe();
        installation.make();
    }

    SECTION("decrease lowers item priority")
    {
        Command decCommand("decrease", { "aaaa" }, {}, "decrease aaaa");
        REQUIRE_NOTHROW(
            PriorityUseCase(
                ioService, decCommand, listItemService, listService, configService, themeService, "decrease")
                .execute());

        ListItemEntity item = listItemService.find("aaaa", listName);
        REQUIRE(*(*item.priority()).getName() == "medium");

        installation.wipe();
        installation.make();
    }

    SECTION("set assigns a specific priority")
    {
        Command setCommand("priority", { "low", "aaaa" }, {}, "priority low aaaa");
        REQUIRE_NOTHROW(
            PriorityUseCase(ioService, setCommand, listItemService, listService, configService, themeService, "set")
                .execute());

        ListItemEntity item = listItemService.find("aaaa", listName);
        REQUIRE(*(*item.priority()).getName() == "low");

        installation.wipe();
        installation.make();
    }

    SECTION("increase with no arguments does not throw")
    {
        Command emptyCommand("increase", {}, {}, "increase");
        REQUIRE_NOTHROW(
            PriorityUseCase(
                ioService, emptyCommand, listItemService, listService, configService, themeService, "increase")
                .execute());
    }
}

// ---------------------------------------------------------------------------
// ResetUseCase
// ---------------------------------------------------------------------------

TEST_CASE("ResetUseCase", "[UseCase][Reset]")
{
    IOService ioService("cli");
    ConfService confService(ioService);
    JSONService jsonService(ioService);
    std::unique_ptr<FileDataServiceInterface> storagePtr = std::make_unique<JSONService>(ioService);
    std::unique_ptr<FileDataServiceInterface> configStoragePtr = std::make_unique<ConfService>(ioService);
    MockInit init(ioService, "_todoos_UseCaseReset");
    MockInstallation installation(ioService, jsonService, confService, init);
    installation.wipe();
    installation.make();

    EventBus bus;
    Command command("reset", { "aaaa" }, {}, "reset aaaa");
    ConfigRepository configRepository(configStoragePtr.get(), init.getConfigFilePath());
    ConfigRepository cacheRepository(configStoragePtr.get(), init.getCacheFilePath());
    ConfigService configService(ioService, init, configRepository, cacheRepository, command);
    PriorityService priorityService;
    StatusService statusService;
    ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
    ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
    ListRepository listRepository(configService, storagePtr.get());
    ListService listService(ioService, configService, listRepository, bus);
    ThemeService themeService(ioService, configService, listService, listItemService);

    SECTION("execute does not throw (cancels when no user input)")
    {
        REQUIRE_NOTHROW(
            ResetUseCase(ioService, command, listItemService, listService, configService, themeService).execute());
    }
}

// ---------------------------------------------------------------------------
// StatusUseCase
// ---------------------------------------------------------------------------

TEST_CASE("StatusUseCase", "[UseCase][Status]")
{
    IOService ioService("cli");
    ConfService confService(ioService);
    JSONService jsonService(ioService);
    std::unique_ptr<FileDataServiceInterface> storagePtr = std::make_unique<JSONService>(ioService);
    std::unique_ptr<FileDataServiceInterface> configStoragePtr = std::make_unique<ConfService>(ioService);
    MockInit init(ioService, "_todoos_UseCaseStatus");
    MockInstallation installation(ioService, jsonService, confService, init);
    installation.wipe();
    installation.make();

    EventBus bus;
    Command command("start", { "aaaa" }, {}, "start aaaa");
    ConfigRepository configRepository(configStoragePtr.get(), init.getConfigFilePath());
    ConfigRepository cacheRepository(configStoragePtr.get(), init.getCacheFilePath());
    ConfigService configService(ioService, init, configRepository, cacheRepository, command);
    PriorityService priorityService;
    StatusService statusService;
    ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
    ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
    ListRepository listRepository(configService, storagePtr.get());
    ListService listService(ioService, configService, listRepository, bus);
    ThemeService themeService(ioService, configService, listService, listItemService);
    ListName listName = listService.createUsedListName();

    SECTION("markAs changes item status")
    {
        REQUIRE_NOTHROW(
            StatusUseCase(
                ioService, command, listItemService, listService, configService, themeService, StatusService::STARTED)
                .execute());

        ListItemEntity item = listItemService.find("aaaa", listName);
        REQUIRE(*(*item.status()).getCommandName() == "started");

        installation.wipe();
        installation.make();
    }

    SECTION("set changes item status from arguments")
    {
        Command setCommand("status", { "paused", "aaaa" }, {}, "status paused aaaa");
        REQUIRE_NOTHROW(
            StatusUseCase(ioService, setCommand, listItemService, listService, configService, themeService, -1)
                .execute());

        ListItemEntity item = listItemService.find("aaaa", listName);
        REQUIRE(*(*item.status()).getCommandName() == "paused");

        installation.wipe();
        installation.make();
    }

    SECTION("markAs with no arguments does not throw")
    {
        Command emptyCommand("start", {}, {}, "start");
        REQUIRE_NOTHROW(StatusUseCase(ioService,
                                      emptyCommand,
                                      listItemService,
                                      listService,
                                      configService,
                                      themeService,
                                      StatusService::STARTED)
                            .execute());
    }
}

// ---------------------------------------------------------------------------
// RemoveUseCase
// ---------------------------------------------------------------------------

TEST_CASE("RemoveUseCase", "[UseCase][Remove]")
{
    IOService ioService("cli");
    ConfService confService(ioService);
    JSONService jsonService(ioService);
    std::unique_ptr<FileDataServiceInterface> storagePtr = std::make_unique<JSONService>(ioService);
    std::unique_ptr<FileDataServiceInterface> configStoragePtr = std::make_unique<ConfService>(ioService);
    MockInit init(ioService, "_todoos_UseCaseRemove");
    MockInstallation installation(ioService, jsonService, confService, init);
    installation.wipe();
    installation.make();

    EventBus bus;
    Command command("remove", { "aaaa" }, {}, "remove aaaa");
    ConfigRepository configRepository(configStoragePtr.get(), init.getConfigFilePath());
    ConfigRepository cacheRepository(configStoragePtr.get(), init.getCacheFilePath());
    ConfigService configService(ioService, init, configRepository, cacheRepository, command);
    PriorityService priorityService;
    StatusService statusService;
    ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
    ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
    ListRepository listRepository(configService, storagePtr.get());
    ListService listService(ioService, configService, listRepository, bus);
    ThemeService themeService(ioService, configService, listService, listItemService);
    ListName listName = listService.createUsedListName();

    SECTION("remove soft-deletes item")
    {
        REQUIRE_NOTHROW(
            RemoveUseCase(ioService, command, listItemService, listService, configService, themeService).execute());

        std::vector<ListItemEntity> items = listItemService.get(listName);
        REQUIRE(items.size() == 1);
        REQUIRE(*items[0].getId() == "bbbb");

        installation.wipe();
        installation.make();
    }

    SECTION("remove with no arguments does not throw")
    {
        Command emptyCommand("remove", {}, {}, "remove");
        REQUIRE_NOTHROW(
            RemoveUseCase(ioService, emptyCommand, listItemService, listService, configService, themeService)
                .execute());

        std::vector<ListItemEntity> items = listItemService.get(listName);
        REQUIRE(items.size() == 2);
    }
}

// ---------------------------------------------------------------------------
// ArchiveUseCase
// ---------------------------------------------------------------------------

TEST_CASE("ArchiveUseCase", "[UseCase][Archive]")
{
    IOService ioService("cli");
    ConfService confService(ioService);
    JSONService jsonService(ioService);
    std::unique_ptr<FileDataServiceInterface> storagePtr = std::make_unique<JSONService>(ioService);
    std::unique_ptr<FileDataServiceInterface> configStoragePtr = std::make_unique<ConfService>(ioService);
    MockInit init(ioService, "_todoos_UseCaseArchive");
    MockInstallation installation(ioService, jsonService, confService, init);
    installation.wipe();
    installation.make();

    EventBus bus;
    Command command("archive", { "aaaa" }, {}, "archive aaaa");
    ConfigRepository configRepository(configStoragePtr.get(), init.getConfigFilePath());
    ConfigRepository cacheRepository(configStoragePtr.get(), init.getCacheFilePath());
    ConfigService configService(ioService, init, configRepository, cacheRepository, command);
    PriorityService priorityService;
    StatusService statusService;
    ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
    ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
    ListRepository listRepository(configService, storagePtr.get());
    ListService listService(ioService, configService, listRepository, bus);
    ThemeService themeService(ioService, configService, listService, listItemService);
    ListName listName = listService.createUsedListName();

    SECTION("archive moves item to archive list")
    {
        REQUIRE_NOTHROW(
            ArchiveUseCase(ioService, command, listItemService, listService, configService, themeService).execute());

        std::vector<ListItemEntity> items = listItemService.get(listName);
        REQUIRE(items.size() == 1);

        ListName archiveName = ListName::createVariant(listName, "archive");
        std::vector<ListItemEntity> archived = listItemService.get(archiveName);
        REQUIRE(archived.size() == 1);
        REQUIRE(*archived[0].getId() == "aaaa");

        installation.wipe();
        installation.make();
    }
}

// ---------------------------------------------------------------------------
// RestoreUseCase
// ---------------------------------------------------------------------------

TEST_CASE("RestoreUseCase", "[UseCase][Restore]")
{
    IOService ioService("cli");
    ConfService confService(ioService);
    JSONService jsonService(ioService);
    std::unique_ptr<FileDataServiceInterface> storagePtr = std::make_unique<JSONService>(ioService);
    std::unique_ptr<FileDataServiceInterface> configStoragePtr = std::make_unique<ConfService>(ioService);
    MockInit init(ioService, "_todoos_UseCaseRestore");
    MockInstallation installation(ioService, jsonService, confService, init);
    installation.wipe();
    installation.make();

    EventBus bus;
    Command command("restore", { "aaaa" }, {}, "restore aaaa");
    ConfigRepository configRepository(configStoragePtr.get(), init.getConfigFilePath());
    ConfigRepository cacheRepository(configStoragePtr.get(), init.getCacheFilePath());
    ConfigService configService(ioService, init, configRepository, cacheRepository, command);
    PriorityService priorityService;
    StatusService statusService;
    ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
    ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
    ListRepository listRepository(configService, storagePtr.get());
    ListService listService(ioService, configService, listRepository, bus);
    ThemeService themeService(ioService, configService, listService, listItemService);
    ListName listName = listService.createUsedListName();

    SECTION("restore brings archived item back to main list")
    {
        listItemService.archive("aaaa", listName);

        REQUIRE_NOTHROW(
            RestoreUseCase(ioService, command, listItemService, listService, configService, themeService).execute());

        std::vector<ListItemEntity> items = listItemService.get(listName);
        REQUIRE(items.size() == 2);

        installation.wipe();
        installation.make();
    }
}

// ---------------------------------------------------------------------------
// ListUseCase
// ---------------------------------------------------------------------------

TEST_CASE("ListUseCase", "[UseCase][List]")
{
    IOService ioService("cli");
    ConfService confService(ioService);
    JSONService jsonService(ioService);
    std::unique_ptr<FileDataServiceInterface> storagePtr = std::make_unique<JSONService>(ioService);
    std::unique_ptr<FileDataServiceInterface> configStoragePtr = std::make_unique<ConfService>(ioService);
    MockInit init(ioService, "_todoos_UseCaseList");
    MockInstallation installation(ioService, jsonService, confService, init);
    installation.wipe();
    installation.make();

    EventBus bus;
    CommandList commandList;
    CommandOption commandOption;
    CommandService commandService(commandList, commandOption);
    Command command("list", {}, {}, "list");
    ConfigRepository configRepository(configStoragePtr.get(), init.getConfigFilePath());
    ConfigRepository cacheRepository(configStoragePtr.get(), init.getCacheFilePath());
    ConfigService configService(ioService, init, configRepository, cacheRepository, command);
    PriorityService priorityService;
    StatusService statusService;
    ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
    ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
    ListRepository listRepository(configService, storagePtr.get());
    ListService listService(ioService, configService, listRepository, bus);
    ThemeService themeService(ioService, configService, listService, listItemService);
    FileStorageService fileStorageService(ioService, configService);

    SECTION("execute does not throw")
    {
        REQUIRE_NOTHROW(ListUseCase(ioService,
                                    command,
                                    commandService,
                                    listService,
                                    listItemService,
                                    fileStorageService,
                                    configService,
                                    themeService)
                            .execute());
    }
}

// ---------------------------------------------------------------------------
// SwitchListUseCase
// ---------------------------------------------------------------------------

TEST_CASE("SwitchListUseCase", "[UseCase][Use]")
{
    IOService ioService("cli");
    ConfService confService(ioService);
    JSONService jsonService(ioService);
    std::unique_ptr<FileDataServiceInterface> storagePtr = std::make_unique<JSONService>(ioService);
    std::unique_ptr<FileDataServiceInterface> configStoragePtr = std::make_unique<ConfService>(ioService);
    MockInit init(ioService, "_todoos_UseCaseUse");
    MockInstallation installation(ioService, jsonService, confService, init);
    installation.wipe();
    installation.make();

    EventBus bus;
    CommandList commandList;
    CommandOption commandOption;
    CommandService commandService(commandList, commandOption);
    Command command("use", { "tempList2Name" }, {}, "use tempList2Name");
    ConfigRepository configRepository(configStoragePtr.get(), init.getConfigFilePath());
    ConfigRepository cacheRepository(configStoragePtr.get(), init.getCacheFilePath());
    ConfigService configService(ioService, init, configRepository, cacheRepository, command);
    PriorityService priorityService;
    StatusService statusService;
    ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
    ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
    ListRepository listRepository(configService, storagePtr.get());
    ListService listService(ioService, configService, listRepository, bus);
    ThemeService themeService(ioService, configService, listService, listItemService);
    FileStorageService fileStorageService(ioService, configService);

    SECTION("execute switches active list without throwing")
    {
        REQUIRE_NOTHROW(SwitchListUseCase(ioService,
                                          command,
                                          commandService,
                                          listService,
                                          listItemService,
                                          fileStorageService,
                                          configService,
                                          themeService)
                            .execute());
    }
}

// ---------------------------------------------------------------------------
// StatsUseCase
// ---------------------------------------------------------------------------

TEST_CASE("StatsUseCase", "[UseCase][Stats]")
{
    IOService ioService("cli");
    ConfService confService(ioService);
    JSONService jsonService(ioService);
    std::unique_ptr<FileDataServiceInterface> storagePtr = std::make_unique<JSONService>(ioService);
    std::unique_ptr<FileDataServiceInterface> configStoragePtr = std::make_unique<ConfService>(ioService);
    MockInit init(ioService, "_todoos_UseCaseStats");
    MockInstallation installation(ioService, jsonService, confService, init);
    installation.wipe();
    installation.make();

    EventBus bus;
    Command command("stats", {}, {}, "stats");
    ConfigRepository configRepository(configStoragePtr.get(), init.getConfigFilePath());
    ConfigRepository cacheRepository(configStoragePtr.get(), init.getCacheFilePath());
    ConfigService configService(ioService, init, configRepository, cacheRepository, command);
    PriorityService priorityService;
    StatusService statusService;
    ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
    ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
    ListRepository listRepository(configService, storagePtr.get());
    ListService listService(ioService, configService, listRepository, bus);
    ThemeService themeService(ioService, configService, listService, listItemService);

    SECTION("execute does not throw")
    {
        REQUIRE_NOTHROW(
            StatsUseCase(ioService, command, configService, listItemService, themeService, listService).execute());
    }
}

// ---------------------------------------------------------------------------
// MoveUseCase
// ---------------------------------------------------------------------------

TEST_CASE("MoveUseCase", "[UseCase][Move]")
{
    IOService ioService("cli");
    ConfService confService(ioService);
    JSONService jsonService(ioService);
    std::unique_ptr<FileDataServiceInterface> storagePtr = std::make_unique<JSONService>(ioService);
    std::unique_ptr<FileDataServiceInterface> configStoragePtr = std::make_unique<ConfService>(ioService);
    MockInit init(ioService, "_todoos_UseCaseMove");
    MockInstallation installation(ioService, jsonService, confService, init);
    installation.wipe();
    installation.make();

    EventBus bus;
    CommandList commandList;
    CommandOption commandOption;
    CommandService commandService(commandList, commandOption);
    Command command("move-to", { "tempList2Name", "aaaa" }, {}, "move-to tempList2Name aaaa");
    ConfigRepository configRepository(configStoragePtr.get(), init.getConfigFilePath());
    ConfigRepository cacheRepository(configStoragePtr.get(), init.getCacheFilePath());
    ConfigService configService(ioService, init, configRepository, cacheRepository, command);
    PriorityService priorityService;
    StatusService statusService;
    ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
    ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
    ListRepository listRepository(configService, storagePtr.get());
    ListService listService(ioService, configService, listRepository, bus);
    ThemeService themeService(ioService, configService, listService, listItemService);

    SECTION("move-to moves item to another list without throwing")
    {
        REQUIRE_NOTHROW(
            MoveUseCase(ioService, command, commandService, listService, listItemService, configService, themeService)
                .execute());

        installation.wipe();
        installation.make();
    }

    SECTION("move-to with no arguments does not throw")
    {
        Command emptyCommand("move-to", {}, {}, "move-to");
        REQUIRE_NOTHROW(
            MoveUseCase(
                ioService, emptyCommand, commandService, listService, listItemService, configService, themeService)
                .execute());
    }
}

// ---------------------------------------------------------------------------
// EmptyUseCase
// ---------------------------------------------------------------------------

TEST_CASE("EmptyUseCase", "[UseCase][Empty]")
{
    IOService ioService("cli");
    ConfService confService(ioService);
    JSONService jsonService(ioService);
    std::unique_ptr<FileDataServiceInterface> storagePtr = std::make_unique<JSONService>(ioService);
    std::unique_ptr<FileDataServiceInterface> configStoragePtr = std::make_unique<ConfService>(ioService);
    MockInit init(ioService, "_todoos_UseCaseEmpty");
    MockInstallation installation(ioService, jsonService, confService, init);
    installation.wipe();
    installation.make();

    EventBus bus;
    Command command("empty", {}, {}, "empty");
    ConfigRepository configRepository(configStoragePtr.get(), init.getConfigFilePath());
    ConfigRepository cacheRepository(configStoragePtr.get(), init.getCacheFilePath());
    ConfigService configService(ioService, init, configRepository, cacheRepository, command);
    PriorityService priorityService;
    StatusService statusService;
    ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
    ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
    ListRepository listRepository(configService, storagePtr.get());
    ListService listService(ioService, configService, listRepository, bus);

    SECTION("execute does not throw (cancels when no user input)")
    {
        REQUIRE_NOTHROW(EmptyUseCase(ioService, command, listItemService, listService, configService).execute());
    }
}

// ---------------------------------------------------------------------------
// CleanUseCase
// ---------------------------------------------------------------------------

TEST_CASE("CleanUseCase", "[UseCase][Clean]")
{
    IOService ioService("cli");
    ConfService confService(ioService);
    JSONService jsonService(ioService);
    std::unique_ptr<FileDataServiceInterface> storagePtr = std::make_unique<JSONService>(ioService);
    std::unique_ptr<FileDataServiceInterface> configStoragePtr = std::make_unique<ConfService>(ioService);
    MockInit init(ioService, "_todoos_UseCaseClean");
    MockInstallation installation(ioService, jsonService, confService, init);
    installation.wipe();
    installation.make();

    EventBus bus;
    Command command("clean", {}, {}, "clean");
    ConfigRepository configRepository(configStoragePtr.get(), init.getConfigFilePath());
    ConfigRepository cacheRepository(configStoragePtr.get(), init.getCacheFilePath());
    ConfigService configService(ioService, init, configRepository, cacheRepository, command);
    PriorityService priorityService;
    StatusService statusService;
    ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
    ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
    ListRepository listRepository(configService, storagePtr.get());
    ListService listService(ioService, configService, listRepository, bus);
    ThemeService themeService(ioService, configService, listService, listItemService);

    SECTION("execute does not throw (cancels when no user input)")
    {
        REQUIRE_NOTHROW(
            CleanUseCase(ioService, command, listItemService, listService, configService, themeService).execute());
    }
}
