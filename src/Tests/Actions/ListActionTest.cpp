#include "../../Actions/ListAction/AddListAction.h"
#include "../../Actions/ListAction/RemoveListAction.h"
#include "../../Actions/ListAction/RenameListAction.h"
#include "../../Actions/ListAction/ShowListAction.h"
#include "../../Command/CommandOption.h"
#include "../../Command/CommandRegistry.h"
#include "../../Command/CommandService.h"
#include "../../Config/ConfigService.h"
#include "../../Events/EventBus.h"
#include "../../FileDataStorageRepositories/ConfigRepository.h"
#include "../../FileDataStorageRepositories/ListItemRepository.h"
#include "../../FileDataStorageRepositories/ListRepository.h"
#include "../../FileStorage/FileStorageService.h"
#include "../../List/ListItemService.h"
#include "../../List/ListItems/PriorityService.h"
#include "../../List/ListItems/StatusService.h"
#include "../../List/ListService.h"
#include "../../Serializers/ConfSerializer.h"
#include "../../Serializers/JsonSerializer.h"
#include "../../Themes/ThemeService.h"
#include "../../UseCase/AddListUseCase.h"
#include "../../UseCase/CopyListUseCase.h"
#include "../../UseCase/MoveListUseCase.h"
#include "../../UseCase/RemoveListUseCase.h"
#include "../../UseCase/RenameListUseCase.h"
#include "../../UseCase/ShowListUseCase.h"
#include "../../UseCase/SwitchListUseCase.h"
#include "../Mock/MockAppInitialization.h"
#include "../Mock/MockAppInstallation.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("ListAction action", "[ListAction]")
{
    IOService ioService("cli");
    ConfSerializer confService(ioService);
    JsonSerializer jsonService(ioService);
    std::unique_ptr<DataSerializerInterface> storagePtr = std::make_unique<JsonSerializer>(ioService);
    std::unique_ptr<DataSerializerInterface> configStoragePtr = std::make_unique<ConfSerializer>(ioService);

    MockAppInitialization init(ioService, "_todoos_ListActionsTest");
    MockAppInstallation installation(ioService, jsonService, confService, init);
    installation.wipe();
    installation.make();

    EventBus bus;
    CommandRegistry commandList;
    CommandOption commandOption;
    CommandService commandService(commandList, commandOption);

    ConfigRepository configRepository(configStoragePtr.get(), init.getConfigFilePath());
    ConfigRepository cacheRepository(configStoragePtr.get(), init.getCacheFilePath());

    PriorityService priorityService;
    StatusService statusService;

    // ---- ShowListAction — show/empty subcommand ----------------------------------------

    SECTION("show — no subcommand shows list without throwing")
    {
        Command command("list", {}, {}, "list");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        std::unique_ptr<DataSerializerInterface> listStoragePtr = std::make_unique<JsonSerializer>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ThemeService themeService(ioService, configService, listService, listItemService);

        ShowListAction action(ioService, listService, configService, themeService);
        REQUIRE_NOTHROW(action.execute(command));
    }

    // ---- AddListAction — add subcommand -----------------------------------------------

    SECTION("add — with name creates new list")
    {
        Command command("list", { "add", "newTestList" }, {}, "list add newTestList");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        std::unique_ptr<DataSerializerInterface> listStoragePtr = std::make_unique<JsonSerializer>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        FileStorageService fileStorageService(ioService, configService);

        std::size_t countBefore = listService.get().size();

        Command subCommand("add", { "newTestList" }, {}, "add newTestList");
        AddListAction action(ioService, listService, fileStorageService);
        REQUIRE_NOTHROW(action.execute(subCommand));

        std::size_t countAfter = listService.get().size();
        REQUIRE(countAfter == countBefore + 1);

        installation.wipe();
        installation.make();
    }

    SECTION("add — with no name does not throw")
    {
        Command command("list", { "add" }, {}, "list add");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        std::unique_ptr<DataSerializerInterface> listStoragePtr = std::make_unique<JsonSerializer>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        FileStorageService fileStorageService(ioService, configService);

        Command subCommand("add", {}, {}, "add");
        AddListAction action(ioService, listService, fileStorageService);
        REQUIRE_NOTHROW(action.execute(subCommand));
    }

    // ---- RemoveListAction — remove subcommand --------------------------------------------

    SECTION("remove — existing list removes it")
    {
        Command command("list", { "remove", "tempList2Name" }, {}, "list remove tempList2Name");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        std::unique_ptr<DataSerializerInterface> listStoragePtr = std::make_unique<JsonSerializer>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        FileStorageService fileStorageService(ioService, configService);

        std::size_t countBefore = listService.get().size();

        Command subCommand("remove", { "tempList2Name" }, {}, "remove tempList2Name");
        RemoveListAction action(ioService, listService, fileStorageService, configService);
        REQUIRE_NOTHROW(action.execute(subCommand));

        std::size_t countAfter = listService.get().size();
        REQUIRE(countAfter == countBefore - 1);

        installation.wipe();
        installation.make();
    }

    SECTION("remove — with no name does not throw")
    {
        Command command("list", { "remove" }, {}, "list remove");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        std::unique_ptr<DataSerializerInterface> listStoragePtr = std::make_unique<JsonSerializer>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        FileStorageService fileStorageService(ioService, configService);

        Command subCommand("remove", {}, {}, "remove");
        RemoveListAction action(ioService, listService, fileStorageService, configService);
        REQUIRE_NOTHROW(action.execute(subCommand));
    }

    // ---- RenameListAction — move-to subcommand -------------------------------------------

    SECTION("rename — move-to renames a list")
    {
        Command command(
            "list", { "move-to", "tempListName", "renamedList" }, {}, "list move-to tempListName renamedList");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        std::unique_ptr<DataSerializerInterface> listStoragePtr = std::make_unique<JsonSerializer>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        FileStorageService fileStorageService(ioService, configService);

        Command subCommand("move-to", { "tempListName", "renamedList" }, {}, "move-to tempListName renamedList");
        RenameListAction action(ioService, listService, fileStorageService, configService);
        REQUIRE_NOTHROW(action.execute(subCommand));

        bool found = false;
        for (auto& list : listService.get()) {
            if (*list.getName() == "renamedList") {
                found = true;
                break;
            }
        }
        REQUIRE(found);

        installation.wipe();
        installation.make();
    }

    SECTION("rename — move-to with too few arguments does not throw")
    {
        Command command("list", { "move-to", "tempListName" }, {}, "list move-to tempListName");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        std::unique_ptr<DataSerializerInterface> listStoragePtr = std::make_unique<JsonSerializer>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        FileStorageService fileStorageService(ioService, configService);

        Command subCommand("move-to", { "tempListName" }, {}, "move-to tempListName");
        RenameListAction action(ioService, listService, fileStorageService, configService);
        REQUIRE_NOTHROW(action.execute(subCommand));
    }

    // ---- ListUseCase — current subcommand ---------------------------------------------

    SECTION("list current — does not throw")
    {
        Command command("list", { "current" }, {}, "list current");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        std::unique_ptr<DataSerializerInterface> listStoragePtr = std::make_unique<JsonSerializer>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        FileStorageService fileStorageService(ioService, configService);
        ThemeService themeService(ioService, configService, listService, listItemService);

        ShowListUseCase useCase(ioService, listService, configService, themeService);
        REQUIRE_NOTHROW(useCase.execute(command));
    }

    // ---- ListUseCase — invalid subcommand ---------------------------------------------

    SECTION("list — invalid subcommand shows error without throwing")
    {
        Command command("list", { "badsubcmd" }, {}, "list badsubcmd");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        std::unique_ptr<DataSerializerInterface> listStoragePtr = std::make_unique<JsonSerializer>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        FileStorageService fileStorageService(ioService, configService);
        ThemeService themeService(ioService, configService, listService, listItemService);

        SwitchListUseCase useCase(
            ioService, commandService, listService, listItemService, fileStorageService, configService, themeService);
        REQUIRE_NOTHROW(useCase.execute(command));
    }

    // ---- SwitchListUseCase ---------------------------------------------------------------

    SECTION("use — with valid list name switches active list")
    {
        Command command("use", { "tempList2Name" }, {}, "use tempList2Name");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        std::unique_ptr<DataSerializerInterface> listStoragePtr = std::make_unique<JsonSerializer>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        FileStorageService fileStorageService(ioService, configService);
        ThemeService themeService(ioService, configService, listService, listItemService);

        SwitchListUseCase useCase(
            ioService, commandService, listService, listItemService, fileStorageService, configService, themeService);
        REQUIRE_NOTHROW(useCase.execute(command));
    }

    SECTION("use — with empty arguments switches to default list")
    {
        Command command("use", {}, {}, "use");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        std::unique_ptr<DataSerializerInterface> listStoragePtr = std::make_unique<JsonSerializer>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        FileStorageService fileStorageService(ioService, configService);
        ThemeService themeService(ioService, configService, listService, listItemService);

        SwitchListUseCase useCase(
            ioService, commandService, listService, listItemService, fileStorageService, configService, themeService);
        REQUIRE_NOTHROW(useCase.execute(command));
    }

    SECTION("use — with non-existent list does not throw")
    {
        Command command("use", { "noSuchList" }, {}, "use noSuchList");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        std::unique_ptr<DataSerializerInterface> listStoragePtr = std::make_unique<JsonSerializer>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        FileStorageService fileStorageService(ioService, configService);
        ThemeService themeService(ioService, configService, listService, listItemService);

        SwitchListUseCase useCase(
            ioService, commandService, listService, listItemService, fileStorageService, configService, themeService);
        REQUIRE_NOTHROW(useCase.execute(command));
    }
}
