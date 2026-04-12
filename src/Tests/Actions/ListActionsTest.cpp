#include "../../Actions/ListActions/ListActions.h"
#include "../../Command/CommandList.h"
#include "../../Command/CommandOption.h"
#include "../../Command/CommandService.h"
#include "../../Config/ConfigService.h"
#include "../../Events/EventBus.h"
#include "../../FileDataStorage/ConfService.h"
#include "../../FileDataStorage/JSONService.h"
#include "../../FileDataStorageRepositories/ConfigRepository.h"
#include "../../FileDataStorageRepositories/ListItemRepository.h"
#include "../../FileDataStorageRepositories/ListRepository.h"
#include "../../FileStorage/FileStorageService.h"
#include "../../List/ListItemService.h"
#include "../../List/ListItems/PriorityService.h"
#include "../../List/ListItems/StatusService.h"
#include "../../List/ListService.h"
#include "../../Themes/ThemeService.h"
#include "../Mock/MockInit.h"
#include "../Mock/MockInstallation.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("ListActions action", "[ListActions]")
{
    IOService ioService("cli");
    ConfService confService(ioService);
    JSONService jsonService(ioService);
    std::unique_ptr<FileDataServiceInterface> storagePtr = std::make_unique<JSONService>(ioService);
    std::unique_ptr<FileDataServiceInterface> configStoragePtr = std::make_unique<ConfService>(ioService);

    MockInit init(ioService, "_todoos_ListActionsTest");
    MockInstallation installation(ioService, jsonService, confService, init);
    installation.wipe();
    installation.make();

    EventBus bus;
    CommandList commandList;
    CommandOption commandOption;
    CommandService commandService(commandList, commandOption);

    ConfigRepository configRepository(configStoragePtr.get(), init.getConfigFilePath());
    ConfigRepository cacheRepository(configStoragePtr.get(), init.getCacheFilePath());

    PriorityService priorityService;
    StatusService statusService;

    // ---- make() — show/empty subcommand ----------------------------------------

    SECTION("make — no subcommand shows list without throwing")
    {
        Command command("list", {}, {}, "list");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        std::unique_ptr<FileDataServiceInterface> listStoragePtr = std::make_unique<JSONService>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        FileStorageService fileStorageService(ioService, configService);
        ThemeService themeService(ioService, configService, listService, listItemService);

        ListActions actions(
            ioService, command, commandService, listService, listItemService, fileStorageService, configService, themeService);
        REQUIRE_NOTHROW(actions.make());
    }

    // ---- make() — add subcommand -----------------------------------------------

    SECTION("make — add with name creates new list")
    {
        Command command("list", { "add", "newTestList" }, {}, "list add newTestList");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        std::unique_ptr<FileDataServiceInterface> listStoragePtr = std::make_unique<JSONService>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        FileStorageService fileStorageService(ioService, configService);
        ThemeService themeService(ioService, configService, listService, listItemService);

        std::size_t countBefore = listService.get().size();

        ListActions actions(
            ioService, command, commandService, listService, listItemService, fileStorageService, configService, themeService);
        REQUIRE_NOTHROW(actions.make());

        std::size_t countAfter = listService.get().size();
        REQUIRE(countAfter == countBefore + 1);

        installation.wipe();
        installation.make();
    }

    SECTION("make — add with no name does not throw")
    {
        Command command("list", { "add" }, {}, "list add");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        std::unique_ptr<FileDataServiceInterface> listStoragePtr = std::make_unique<JSONService>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        FileStorageService fileStorageService(ioService, configService);
        ThemeService themeService(ioService, configService, listService, listItemService);

        ListActions actions(
            ioService, command, commandService, listService, listItemService, fileStorageService, configService, themeService);
        REQUIRE_NOTHROW(actions.make());
    }

    // ---- make() — remove subcommand --------------------------------------------

    SECTION("make — remove existing list removes it")
    {
        Command command("list", { "remove", "tempList2Name" }, {}, "list remove tempList2Name");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        std::unique_ptr<FileDataServiceInterface> listStoragePtr = std::make_unique<JSONService>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        FileStorageService fileStorageService(ioService, configService);
        ThemeService themeService(ioService, configService, listService, listItemService);

        std::size_t countBefore = listService.get().size();

        ListActions actions(
            ioService, command, commandService, listService, listItemService, fileStorageService, configService, themeService);
        REQUIRE_NOTHROW(actions.make());

        std::size_t countAfter = listService.get().size();
        REQUIRE(countAfter == countBefore - 1);

        installation.wipe();
        installation.make();
    }

    SECTION("make — remove with no name does not throw")
    {
        Command command("list", { "remove" }, {}, "list remove");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        std::unique_ptr<FileDataServiceInterface> listStoragePtr = std::make_unique<JSONService>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        FileStorageService fileStorageService(ioService, configService);
        ThemeService themeService(ioService, configService, listService, listItemService);

        ListActions actions(
            ioService, command, commandService, listService, listItemService, fileStorageService, configService, themeService);
        REQUIRE_NOTHROW(actions.make());
    }

    // ---- make() — move-to subcommand -------------------------------------------

    SECTION("make — move-to renames a list")
    {
        Command command("list", { "move-to", "tempListName", "renamedList" }, {}, "list move-to tempListName renamedList");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        std::unique_ptr<FileDataServiceInterface> listStoragePtr = std::make_unique<JSONService>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        FileStorageService fileStorageService(ioService, configService);
        ThemeService themeService(ioService, configService, listService, listItemService);

        ListActions actions(
            ioService, command, commandService, listService, listItemService, fileStorageService, configService, themeService);
        REQUIRE_NOTHROW(actions.make());

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

    SECTION("make — move-to with too few arguments does not throw")
    {
        Command command("list", { "move-to", "tempListName" }, {}, "list move-to tempListName");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        std::unique_ptr<FileDataServiceInterface> listStoragePtr = std::make_unique<JSONService>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        FileStorageService fileStorageService(ioService, configService);
        ThemeService themeService(ioService, configService, listService, listItemService);

        ListActions actions(
            ioService, command, commandService, listService, listItemService, fileStorageService, configService, themeService);
        REQUIRE_NOTHROW(actions.make());
    }

    // ---- make() — current subcommand -------------------------------------------

    SECTION("make — current does not throw")
    {
        Command command("list", { "current" }, {}, "list current");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        std::unique_ptr<FileDataServiceInterface> listStoragePtr = std::make_unique<JSONService>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        FileStorageService fileStorageService(ioService, configService);
        ThemeService themeService(ioService, configService, listService, listItemService);

        ListActions actions(
            ioService, command, commandService, listService, listItemService, fileStorageService, configService, themeService);
        REQUIRE_NOTHROW(actions.make());
    }

    // ---- make() — invalid subcommand -------------------------------------------

    SECTION("make — invalid subcommand throws std::invalid_argument")
    {
        Command command("list", { "badsubcmd" }, {}, "list badsubcmd");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        std::unique_ptr<FileDataServiceInterface> listStoragePtr = std::make_unique<JSONService>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        FileStorageService fileStorageService(ioService, configService);
        ThemeService themeService(ioService, configService, listService, listItemService);

        ListActions actions(
            ioService, command, commandService, listService, listItemService, fileStorageService, configService, themeService);
        REQUIRE_THROWS_AS(actions.make(), std::invalid_argument);
    }

    // ---- use() -----------------------------------------------------------------

    SECTION("use — with valid list name switches active list")
    {
        Command command("use", { "tempList2Name" }, {}, "use tempList2Name");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        std::unique_ptr<FileDataServiceInterface> listStoragePtr = std::make_unique<JSONService>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        FileStorageService fileStorageService(ioService, configService);
        ThemeService themeService(ioService, configService, listService, listItemService);

        ListActions actions(
            ioService, command, commandService, listService, listItemService, fileStorageService, configService, themeService);
        REQUIRE_NOTHROW(actions.use());
    }

    SECTION("use — with empty arguments switches to default list")
    {
        Command command("use", {}, {}, "use");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        std::unique_ptr<FileDataServiceInterface> listStoragePtr = std::make_unique<JSONService>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        FileStorageService fileStorageService(ioService, configService);
        ThemeService themeService(ioService, configService, listService, listItemService);

        ListActions actions(
            ioService, command, commandService, listService, listItemService, fileStorageService, configService, themeService);
        REQUIRE_NOTHROW(actions.use());
    }

    SECTION("use — with non-existent list does not throw")
    {
        Command command("use", { "noSuchList" }, {}, "use noSuchList");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        std::unique_ptr<FileDataServiceInterface> listStoragePtr = std::make_unique<JSONService>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        FileStorageService fileStorageService(ioService, configService);
        ThemeService themeService(ioService, configService, listService, listItemService);

        ListActions actions(
            ioService, command, commandService, listService, listItemService, fileStorageService, configService, themeService);
        REQUIRE_NOTHROW(actions.use());
    }
}
