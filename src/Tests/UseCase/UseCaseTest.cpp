#include "../../Command/CommandOption.h"
#include "../../Command/CommandRegistry.h"
#include "../../Command/CommandService.h"
#include "../../Events/EventBus.h"
#include "../../FileDataStorageRepositories/ListItemRepository.h"
#include "../../FileDataStorageRepositories/ListRepository.h"
#include "../../FileStorage/FileStorageService.h"
#include "../../Help/HelpPrinter.h"
#include "../../Serializers/ConfSerializer.h"
#include "../../Serializers/JsonSerializer.h"
#include "../../Themes/ThemeService.h"
#include "../../UseCase/AddItemUseCase.h"
#include "../../UseCase/AddListUseCase.h"
#include "../../UseCase/ArchiveUseCase.h"
#include "../../UseCase/CleanUseCase.h"
#include "../../UseCase/CopyListUseCase.h"
#include "../../UseCase/EmptyUseCase.h"
#include "../../UseCase/FindUseCase.h"
#include "../../UseCase/MoveItemUseCase.h"
#include "../../UseCase/MoveListUseCase.h"
#include "../../UseCase/PriorityUseCase.h"
#include "../../UseCase/RemoveListUseCase.h"
#include "../../UseCase/RemoveUseCase.h"
#include "../../UseCase/RenameListUseCase.h"
#include "../../UseCase/ResetUseCase.h"
#include "../../UseCase/RestoreUseCase.h"
#include "../../UseCase/ShowUseCase.h"
#include "../../UseCase/StatsUseCase.h"
#include "../../UseCase/StatusUseCase.h"
#include "../../UseCase/ShowListUseCase.h"
#include "../../UseCase/SwitchListUseCase.h"
#include "../Mock/MockAppInitialization.h"
#include "../Mock/MockAppInstallation.h"
#include <catch2/catch_test_macros.hpp>

// ---------------------------------------------------------------------------
// ShowUseCase
// ---------------------------------------------------------------------------

TEST_CASE("ShowUseCase", "[UseCase][Show]")
{
    IOService ioService("cli");
    ConfSerializer confService(ioService);
    JsonSerializer jsonService(ioService);
    std::unique_ptr<DataSerializerInterface> storagePtr = std::make_unique<JsonSerializer>(ioService);
    std::unique_ptr<DataSerializerInterface> configStoragePtr = std::make_unique<ConfSerializer>(ioService);
    MockAppInitialization init(ioService, "_todoos_UseCaseShow");
    MockAppInstallation installation(ioService, jsonService, confService, init);
    installation.wipe();
    installation.make();

    EventBus bus;
    CommandRegistry commandList;
    CommandOption commandOption;
    CommandService commandService(commandList, commandOption);
    HelpPrinter help(ioService);
    ConfigRepository configRepository(configStoragePtr.get(), init.getConfigFilePath());
    ConfigRepository cacheRepository(configStoragePtr.get(), init.getCacheFilePath());

    SECTION("execute with default command does not throw")
    {
        Command command("show", {}, {}, "show");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService;
        StatusService statusService;
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        ListRepository listRepository(configService, storagePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ThemeService themeService(ioService, configService, listService, listItemService);

        REQUIRE_NOTHROW(
            ShowUseCase(ioService, help, commandService, configService, listService, listItemService, themeService)
                .execute(command));
    }

    SECTION("execute with named list does not throw")
    {
        Command command("show", { "tempListName" }, {}, "show tempListName");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService;
        StatusService statusService;
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        ListRepository listRepository(configService, storagePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ThemeService themeService(ioService, configService, listService, listItemService);

        REQUIRE_NOTHROW(
            ShowUseCase(ioService, help, commandService, configService, listService, listItemService, themeService)
                .execute(command));
    }
}

// ---------------------------------------------------------------------------
// AddItemUseCase
// ---------------------------------------------------------------------------

TEST_CASE("AddItemUseCase", "[UseCase][AddItem]")
{
    IOService ioService("cli");
    ConfSerializer confService(ioService);
    JsonSerializer jsonService(ioService);
    std::unique_ptr<DataSerializerInterface> storagePtr = std::make_unique<JsonSerializer>(ioService);
    std::unique_ptr<DataSerializerInterface> configStoragePtr = std::make_unique<ConfSerializer>(ioService);
    MockAppInitialization init(ioService, "_todoos_UseCaseAddItem");
    MockAppInstallation installation(ioService, jsonService, confService, init);
    installation.wipe();
    installation.make();

    EventBus bus;
    CommandRegistry commandList;
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
            AddItemUseCase(ioService, commandService, listItemService, listService, configService, themeService)
                .execute(command));

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
            AddItemUseCase(ioService, commandService, listItemService, listService, configService, themeService)
                .execute(command));

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
    ConfSerializer confService(ioService);
    JsonSerializer jsonService(ioService);
    std::unique_ptr<DataSerializerInterface> storagePtr = std::make_unique<JsonSerializer>(ioService);
    std::unique_ptr<DataSerializerInterface> configStoragePtr = std::make_unique<ConfSerializer>(ioService);
    MockAppInitialization init(ioService, "_todoos_UseCaseFind");
    MockAppInstallation installation(ioService, jsonService, confService, init);
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
            FindUseCase(ioService, configService, listService, listItemService, themeService).execute(command));
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
            FindUseCase(ioService, configService, listService, listItemService, themeService).execute(command));
    }
}

// ---------------------------------------------------------------------------
// PriorityUseCase
// ---------------------------------------------------------------------------

TEST_CASE("PriorityUseCase", "[UseCase][Priority]")
{
    IOService ioService("cli");
    ConfSerializer confService(ioService);
    JsonSerializer jsonService(ioService);
    std::unique_ptr<DataSerializerInterface> storagePtr = std::make_unique<JsonSerializer>(ioService);
    std::unique_ptr<DataSerializerInterface> configStoragePtr = std::make_unique<ConfSerializer>(ioService);
    MockAppInitialization init(ioService, "_todoos_UseCasePriority");
    MockAppInstallation installation(ioService, jsonService, confService, init);
    installation.wipe();
    installation.make();

    EventBus bus;
    ConfigRepository configRepository(configStoragePtr.get(), init.getConfigFilePath());
    ConfigRepository cacheRepository(configStoragePtr.get(), init.getCacheFilePath());

    SECTION("increase raises item priority")
    {
        Command command("increase", { "aaaa" }, {}, "increase aaaa");
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
            PriorityUseCase(ioService, listItemService, listService, configService, themeService)
                .execute(command, "increase"));

        ListItemEntity item = listItemService.find("aaaa", listName);
        REQUIRE(*(*item.priority()).getName() == "urgent");

        installation.wipe();
        installation.make();
    }

    SECTION("decrease lowers item priority")
    {
        Command command("decrease", { "aaaa" }, {}, "decrease aaaa");
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
            PriorityUseCase(ioService, listItemService, listService, configService, themeService)
                .execute(command, "decrease"));

        ListItemEntity item = listItemService.find("aaaa", listName);
        REQUIRE(*(*item.priority()).getName() == "medium");

        installation.wipe();
        installation.make();
    }

    SECTION("set assigns a specific priority")
    {
        Command command("priority", { "low", "aaaa" }, {}, "priority low aaaa");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService;
        StatusService statusService;
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        ListRepository listRepository(configService, storagePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ThemeService themeService(ioService, configService, listService, listItemService);
        ListName listName = listService.createUsedListName();

        REQUIRE_NOTHROW(PriorityUseCase(ioService, listItemService, listService, configService, themeService)
                            .execute(command, "set"));

        ListItemEntity item = listItemService.find("aaaa", listName);
        REQUIRE(*(*item.priority()).getName() == "low");

        installation.wipe();
        installation.make();
    }

    SECTION("increase with no arguments does not throw")
    {
        Command command("increase", {}, {}, "increase");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService;
        StatusService statusService;
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        ListRepository listRepository(configService, storagePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ThemeService themeService(ioService, configService, listService, listItemService);

        REQUIRE_NOTHROW(
            PriorityUseCase(ioService, listItemService, listService, configService, themeService)
                .execute(command, "increase"));
    }
}

// ---------------------------------------------------------------------------
// ResetUseCase
// ---------------------------------------------------------------------------

TEST_CASE("ResetUseCase", "[UseCase][Reset]")
{
    IOService ioService("cli");
    ConfSerializer confService(ioService);
    JsonSerializer jsonService(ioService);
    std::unique_ptr<DataSerializerInterface> storagePtr = std::make_unique<JsonSerializer>(ioService);
    std::unique_ptr<DataSerializerInterface> configStoragePtr = std::make_unique<ConfSerializer>(ioService);
    MockAppInitialization init(ioService, "_todoos_UseCaseReset");
    MockAppInstallation installation(ioService, jsonService, confService, init);
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
            ResetUseCase(ioService, listItemService, listService, configService, themeService).execute(command));
    }
}

// ---------------------------------------------------------------------------
// StatusUseCase
// ---------------------------------------------------------------------------

TEST_CASE("StatusUseCase", "[UseCase][Status]")
{
    IOService ioService("cli");
    ConfSerializer confService(ioService);
    JsonSerializer jsonService(ioService);
    std::unique_ptr<DataSerializerInterface> storagePtr = std::make_unique<JsonSerializer>(ioService);
    std::unique_ptr<DataSerializerInterface> configStoragePtr = std::make_unique<ConfSerializer>(ioService);
    MockAppInitialization init(ioService, "_todoos_UseCaseStatus");
    MockAppInstallation installation(ioService, jsonService, confService, init);
    installation.wipe();
    installation.make();

    EventBus bus;
    ConfigRepository configRepository(configStoragePtr.get(), init.getConfigFilePath());
    ConfigRepository cacheRepository(configStoragePtr.get(), init.getCacheFilePath());

    SECTION("markAs changes item status")
    {
        Command command("start", { "aaaa" }, {}, "start aaaa");
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
            StatusUseCase(ioService, listItemService, listService, configService, themeService)
                .execute(command, StatusService::STARTED));

        ListItemEntity item = listItemService.find("aaaa", listName);
        REQUIRE(*(*item.status()).getCommandName() == "started");

        installation.wipe();
        installation.make();
    }

    SECTION("set changes item status from arguments")
    {
        Command command("status", { "paused", "aaaa" }, {}, "status paused aaaa");
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
            StatusUseCase(ioService, listItemService, listService, configService, themeService).execute(command, -1));

        ListItemEntity item = listItemService.find("aaaa", listName);
        REQUIRE(*(*item.status()).getCommandName() == "paused");

        installation.wipe();
        installation.make();
    }

    SECTION("markAs with no arguments does not throw")
    {
        Command command("start", {}, {}, "start");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService;
        StatusService statusService;
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        ListRepository listRepository(configService, storagePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ThemeService themeService(ioService, configService, listService, listItemService);

        REQUIRE_NOTHROW(
            StatusUseCase(ioService, listItemService, listService, configService, themeService)
                .execute(command, StatusService::STARTED));
    }
}

// ---------------------------------------------------------------------------
// RemoveUseCase
// ---------------------------------------------------------------------------

TEST_CASE("RemoveUseCase", "[UseCase][Remove]")
{
    IOService ioService("cli");
    ConfSerializer confService(ioService);
    JsonSerializer jsonService(ioService);
    std::unique_ptr<DataSerializerInterface> storagePtr = std::make_unique<JsonSerializer>(ioService);
    std::unique_ptr<DataSerializerInterface> configStoragePtr = std::make_unique<ConfSerializer>(ioService);
    MockAppInitialization init(ioService, "_todoos_UseCaseRemove");
    MockAppInstallation installation(ioService, jsonService, confService, init);
    installation.wipe();
    installation.make();

    EventBus bus;
    ConfigRepository configRepository(configStoragePtr.get(), init.getConfigFilePath());
    ConfigRepository cacheRepository(configStoragePtr.get(), init.getCacheFilePath());

    SECTION("remove soft-deletes item")
    {
        Command command("remove", { "aaaa" }, {}, "remove aaaa");
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
            RemoveUseCase(ioService, listItemService, listService, configService, themeService).execute(command));

        std::vector<ListItemEntity> items = listItemService.get(listName);
        REQUIRE(items.size() == 1);
        REQUIRE(*items[0].getId() == "bbbb");

        installation.wipe();
        installation.make();
    }

    SECTION("remove with no arguments does not throw")
    {
        Command command("remove", {}, {}, "remove");
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
            RemoveUseCase(ioService, listItemService, listService, configService, themeService).execute(command));

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
    ConfSerializer confService(ioService);
    JsonSerializer jsonService(ioService);
    std::unique_ptr<DataSerializerInterface> storagePtr = std::make_unique<JsonSerializer>(ioService);
    std::unique_ptr<DataSerializerInterface> configStoragePtr = std::make_unique<ConfSerializer>(ioService);
    MockAppInitialization init(ioService, "_todoos_UseCaseArchive");
    MockAppInstallation installation(ioService, jsonService, confService, init);
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
            ArchiveUseCase(ioService, listItemService, listService, configService, themeService).execute(command));

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
    ConfSerializer confService(ioService);
    JsonSerializer jsonService(ioService);
    std::unique_ptr<DataSerializerInterface> storagePtr = std::make_unique<JsonSerializer>(ioService);
    std::unique_ptr<DataSerializerInterface> configStoragePtr = std::make_unique<ConfSerializer>(ioService);
    MockAppInitialization init(ioService, "_todoos_UseCaseRestore");
    MockAppInstallation installation(ioService, jsonService, confService, init);
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
            RestoreUseCase(ioService, listItemService, listService, configService, themeService).execute(command));

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
    ConfSerializer confService(ioService);
    JsonSerializer jsonService(ioService);
    std::unique_ptr<DataSerializerInterface> storagePtr = std::make_unique<JsonSerializer>(ioService);
    std::unique_ptr<DataSerializerInterface> configStoragePtr = std::make_unique<ConfSerializer>(ioService);
    MockAppInitialization init(ioService, "_todoos_UseCaseList");
    MockAppInstallation installation(ioService, jsonService, confService, init);
    installation.wipe();
    installation.make();

    EventBus bus;
    CommandRegistry commandList;
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
        REQUIRE_NOTHROW(
            ShowListUseCase(ioService, listService, configService, themeService).execute(command));
    }
}

// ---------------------------------------------------------------------------
// SwitchListUseCase (use command)
// ---------------------------------------------------------------------------

TEST_CASE("SwitchListUseCase", "[UseCase][Use]")
{
    IOService ioService("cli");
    ConfSerializer confService(ioService);
    JsonSerializer jsonService(ioService);
    std::unique_ptr<DataSerializerInterface> storagePtr = std::make_unique<JsonSerializer>(ioService);
    std::unique_ptr<DataSerializerInterface> configStoragePtr = std::make_unique<ConfSerializer>(ioService);
    MockAppInitialization init(ioService, "_todoos_UseCaseUse");
    MockAppInstallation installation(ioService, jsonService, confService, init);
    installation.wipe();
    installation.make();

    EventBus bus;
    CommandRegistry commandList;
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
        REQUIRE_NOTHROW(SwitchListUseCase(
                             ioService, commandService, listService, listItemService, fileStorageService, configService, themeService)
                             .execute(command));
    }
}

// ---------------------------------------------------------------------------
// StatsUseCase
// ---------------------------------------------------------------------------

TEST_CASE("StatsUseCase", "[UseCase][Stats]")
{
    IOService ioService("cli");
    ConfSerializer confService(ioService);
    JsonSerializer jsonService(ioService);
    std::unique_ptr<DataSerializerInterface> storagePtr = std::make_unique<JsonSerializer>(ioService);
    std::unique_ptr<DataSerializerInterface> configStoragePtr = std::make_unique<ConfSerializer>(ioService);
    MockAppInitialization init(ioService, "_todoos_UseCaseStats");
    MockAppInstallation installation(ioService, jsonService, confService, init);
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
            StatsUseCase(ioService, configService, listItemService, themeService, listService).execute(command));
    }
}

// ---------------------------------------------------------------------------
// MoveItemUseCase
// ---------------------------------------------------------------------------

TEST_CASE("MoveItemUseCase", "[UseCase][Move]")
{
    IOService ioService("cli");
    ConfSerializer confService(ioService);
    JsonSerializer jsonService(ioService);
    std::unique_ptr<DataSerializerInterface> storagePtr = std::make_unique<JsonSerializer>(ioService);
    std::unique_ptr<DataSerializerInterface> configStoragePtr = std::make_unique<ConfSerializer>(ioService);
    MockAppInitialization init(ioService, "_todoos_UseCaseMove");
    MockAppInstallation installation(ioService, jsonService, confService, init);
    installation.wipe();
    installation.make();

    EventBus bus;
    CommandRegistry commandList;
    CommandOption commandOption;
    CommandService commandService(commandList, commandOption);
    ConfigRepository configRepository(configStoragePtr.get(), init.getConfigFilePath());
    ConfigRepository cacheRepository(configStoragePtr.get(), init.getCacheFilePath());

    SECTION("move-to moves item to another list without throwing")
    {
        Command command("move-to", { "tempList2Name", "aaaa" }, {}, "move-to tempList2Name aaaa");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService;
        StatusService statusService;
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        ListRepository listRepository(configService, storagePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ThemeService themeService(ioService, configService, listService, listItemService);

        REQUIRE_NOTHROW(
            MoveItemUseCase(ioService, commandService, listService, listItemService, configService, themeService).execute(command));

        installation.wipe();
        installation.make();
    }

    SECTION("move-to with no arguments does not throw")
    {
        Command command("move-to", {}, {}, "move-to");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        PriorityService priorityService;
        StatusService statusService;
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        ListRepository listRepository(configService, storagePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ThemeService themeService(ioService, configService, listService, listItemService);

        REQUIRE_NOTHROW(
            MoveItemUseCase(ioService, commandService, listService, listItemService, configService, themeService).execute(command));
    }
}

// ---------------------------------------------------------------------------
// EmptyUseCase
// ---------------------------------------------------------------------------

TEST_CASE("EmptyUseCase", "[UseCase][Empty]")
{
    IOService ioService("cli");
    ConfSerializer confService(ioService);
    JsonSerializer jsonService(ioService);
    std::unique_ptr<DataSerializerInterface> storagePtr = std::make_unique<JsonSerializer>(ioService);
    std::unique_ptr<DataSerializerInterface> configStoragePtr = std::make_unique<ConfSerializer>(ioService);
    MockAppInitialization init(ioService, "_todoos_UseCaseEmpty");
    MockAppInstallation installation(ioService, jsonService, confService, init);
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
    ThemeService themeService(ioService, configService, listService, listItemService);

    SECTION("execute does not throw (cancels when no user input)")
    {
        REQUIRE_NOTHROW(
            EmptyUseCase(ioService, listItemService, listService, configService, themeService).execute(command));
    }
}

// ---------------------------------------------------------------------------
// CleanUseCase
// ---------------------------------------------------------------------------

TEST_CASE("CleanUseCase", "[UseCase][Clean]")
{
    IOService ioService("cli");
    ConfSerializer confService(ioService);
    JsonSerializer jsonService(ioService);
    std::unique_ptr<DataSerializerInterface> storagePtr = std::make_unique<JsonSerializer>(ioService);
    std::unique_ptr<DataSerializerInterface> configStoragePtr = std::make_unique<ConfSerializer>(ioService);
    MockAppInitialization init(ioService, "_todoos_UseCaseClean");
    MockAppInstallation installation(ioService, jsonService, confService, init);
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
            CleanUseCase(ioService, listItemService, listService, configService, themeService).execute(command));
    }
}
