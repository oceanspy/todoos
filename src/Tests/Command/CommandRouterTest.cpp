#include "../../Command/CommandRouter.h"
#include "../../Command/CommandOption.h"
#include "../../Command/CommandRegistry.h"
#include "../../Command/CommandService.h"
#include "../../Config/ConfigService.h"
#include "../../Events/EventBus.h"
#include "../../FileDataStorageRepositories/ConfigRepository.h"
#include "../../FileDataStorageRepositories/ListItemRepository.h"
#include "../../FileDataStorageRepositories/ListRepository.h"
#include "../../FileStorage/FileStorageService.h"
#include "../../Help/HelpPrinter.h"
#include "../../List/ListItemService.h"
#include "../../List/ListItems/PriorityService.h"
#include "../../List/ListItems/StatusService.h"
#include "../../List/ListService.h"
#include "../../Serializers/ConfSerializer.h"
#include "../../Serializers/JsonSerializer.h"
#include "../../Themes/ThemeService.h"
#include "../Mock/MockAppInitialization.h"
#include "../Mock/MockAppInstallation.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("CommandRouter", "[CommandRouter]")
{
    IOService ioService("cli");
    ConfSerializer confService(ioService);
    JsonSerializer jsonService(ioService);
    std::unique_ptr<DataSerializerInterface> storagePtr = std::make_unique<JsonSerializer>(ioService);
    std::unique_ptr<DataSerializerInterface> configStoragePtr = std::make_unique<ConfSerializer>(ioService);

    MockAppInitialization init(ioService, "_todoos_CommandRouterRouteTest");
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

    PriorityService priorityService;
    StatusService statusService;

    // ---- show ------------------------------------------------------------------

    SECTION("show command routes to ShowUseCase without throwing")
    {
        Command command("show", {}, {}, "show");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        std::unique_ptr<DataSerializerInterface> listStoragePtr = std::make_unique<JsonSerializer>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        FileStorageService fileStorageService(ioService, configService);
        ThemeService themeService(ioService, configService, listService, listItemService);

        CommandRouter router(ioService,
                             help,
                             commandService,
                             configService,
                             fileStorageService,
                             listService,
                             listItemService,
                             themeService);
        REQUIRE_NOTHROW(router.execute(command));
    }

    // ---- add -------------------------------------------------------------------

    SECTION("add command routes to ListItemActionsUseCase and adds item")
    {
        Command command("add", { "routed", "item" }, {}, "add routed item");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        std::unique_ptr<DataSerializerInterface> listStoragePtr = std::make_unique<JsonSerializer>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        FileStorageService fileStorageService(ioService, configService);
        ThemeService themeService(ioService, configService, listService, listItemService);
        ListName listName = listService.createUsedListName();

        CommandRouter router(ioService,
                             help,
                             commandService,
                             configService,
                             fileStorageService,
                             listService,
                             listItemService,
                             themeService);
        REQUIRE_NOTHROW(router.execute(command));

        std::vector<ListItemEntity> items = listItemService.get(listName);
        REQUIRE(items.size() == 3);

        installation.wipe();
        installation.make();
    }

    // ---- remove ----------------------------------------------------------------

    SECTION("remove command routes to RemoveUseCase and removes item")
    {
        Command command("remove", { "aaaa" }, {}, "remove aaaa");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        std::unique_ptr<DataSerializerInterface> listStoragePtr = std::make_unique<JsonSerializer>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        FileStorageService fileStorageService(ioService, configService);
        ThemeService themeService(ioService, configService, listService, listItemService);
        ListName listName = listService.createUsedListName();

        CommandRouter router(ioService,
                             help,
                             commandService,
                             configService,
                             fileStorageService,
                             listService,
                             listItemService,
                             themeService);
        REQUIRE_NOTHROW(router.execute(command));

        std::vector<ListItemEntity> items = listItemService.get(listName);
        REQUIRE(items.size() == 1);

        installation.wipe();
        installation.make();
    }

    // ---- start -----------------------------------------------------------------

    SECTION("start command routes to StatusUseCase and changes status")
    {
        Command command("start", { "aaaa" }, {}, "start aaaa");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        std::unique_ptr<DataSerializerInterface> listStoragePtr = std::make_unique<JsonSerializer>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        FileStorageService fileStorageService(ioService, configService);
        ThemeService themeService(ioService, configService, listService, listItemService);
        ListName listName = listService.createUsedListName();

        CommandRouter router(ioService,
                             help,
                             commandService,
                             configService,
                             fileStorageService,
                             listService,
                             listItemService,
                             themeService);
        REQUIRE_NOTHROW(router.execute(command));

        ListItemEntity item = listItemService.find("aaaa", listName);
        REQUIRE(*(*item.status()).getCommandName() == "started");

        installation.wipe();
        installation.make();
    }

    // ---- increase --------------------------------------------------------------

    SECTION("increase command routes to PriorityUseCase and raises priority")
    {
        Command command("increase", { "aaaa" }, {}, "increase aaaa");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        std::unique_ptr<DataSerializerInterface> listStoragePtr = std::make_unique<JsonSerializer>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        FileStorageService fileStorageService(ioService, configService);
        ThemeService themeService(ioService, configService, listService, listItemService);
        ListName listName = listService.createUsedListName();

        CommandRouter router(ioService,
                             help,
                             commandService,
                             configService,
                             fileStorageService,
                             listService,
                             listItemService,
                             themeService);
        REQUIRE_NOTHROW(router.execute(command));

        ListItemEntity item = listItemService.find("aaaa", listName);
        REQUIRE(*(*item.priority()).getName() == "urgent");

        installation.wipe();
        installation.make();
    }

    // ---- stats -----------------------------------------------------------------

    SECTION("stats command routes to StatsUseCase without throwing")
    {
        Command command("stats", {}, {}, "stats");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        std::unique_ptr<DataSerializerInterface> listStoragePtr = std::make_unique<JsonSerializer>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        FileStorageService fileStorageService(ioService, configService);
        ThemeService themeService(ioService, configService, listService, listItemService);

        CommandRouter router(ioService,
                             help,
                             commandService,
                             configService,
                             fileStorageService,
                             listService,
                             listItemService,
                             themeService);
        REQUIRE_NOTHROW(router.execute(command));
    }

    // ---- unknown command -------------------------------------------------------

    SECTION("unknown command throws std::invalid_argument")
    {
        // CommandRegistry::getCommandId throws for unregistered command names
        Command command("unknowncommandxyz", {}, {}, "unknowncommandxyz");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
        std::unique_ptr<DataSerializerInterface> listStoragePtr = std::make_unique<JsonSerializer>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        FileStorageService fileStorageService(ioService, configService);
        ThemeService themeService(ioService, configService, listService, listItemService);

        CommandRouter router(ioService,
                             help,
                             commandService,
                             configService,
                             fileStorageService,
                             listService,
                             listItemService,
                             themeService);
        REQUIRE_THROWS_AS(router.execute(command), std::invalid_argument);
    }
}
