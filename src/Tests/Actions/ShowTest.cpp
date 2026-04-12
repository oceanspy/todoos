#include "../../Actions/Show/Show.h"
#include "../../Config/ConfigService.h"
#include "../../Events/EventBus.h"
#include "../../FileDataStorage/ConfService.h"
#include "../../FileDataStorage/JSONService.h"
#include "../../FileDataStorageRepositories/ConfigRepository.h"
#include "../../FileDataStorageRepositories/ListItemRepository.h"
#include "../../FileDataStorageRepositories/ListRepository.h"
#include "../../List/ListItemService.h"
#include "../../List/ListItems/PriorityService.h"
#include "../../List/ListItems/StatusService.h"
#include "../../List/ListService.h"
#include "../../Themes/ThemeService.h"
#include "../Mock/MockInit.h"
#include "../Mock/MockInstallation.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Show action", "[Show]")
{
    IOService ioService("cli");
    ConfService confService(ioService);
    JSONService jsonService(ioService);
    std::unique_ptr<FileDataServiceInterface> storagePtr = std::make_unique<JSONService>(ioService);
    std::unique_ptr<FileDataServiceInterface> configStoragePtr = std::make_unique<ConfService>(ioService);

    MockInit init(ioService, "_todoos_ShowActionTest");
    MockInstallation installation(ioService, jsonService, confService, init);
    installation.wipe();
    installation.make();

    EventBus bus;
    Command command("show", {}, {}, "show");
    ConfigRepository configRepository(configStoragePtr.get(), init.getConfigFilePath());
    ConfigRepository cacheRepository(configStoragePtr.get(), init.getCacheFilePath());
    ConfigService configService(ioService, init, configRepository, cacheRepository, command);

    PriorityService priorityService;
    StatusService statusService;
    ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
    ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
    std::unique_ptr<FileDataServiceInterface> listStoragePtr = std::make_unique<JSONService>(ioService);
    ListRepository listRepository(configService, listStoragePtr.get());
    ListService listService(ioService, configService, listRepository, bus);
    ThemeService themeService(ioService, configService, listService, listItemService);

    ListName listName = listService.createUsedListName();

    // ---- print -----------------------------------------------------------------

    SECTION("print — with populated items does not throw")
    {
        std::vector<ListItemEntity> items = listItemService.get(listName);
        Show show(ioService, listService, listItemService, themeService);
        REQUIRE_NOTHROW(show.print(items, listName));
    }

    SECTION("print — with empty items does not throw")
    {
        std::vector<ListItemEntity> items;
        Show show(ioService, listService, listItemService, themeService);
        REQUIRE_NOTHROW(show.print(items, listName));
    }

    SECTION("print — showListName=false does not throw")
    {
        std::vector<ListItemEntity> items = listItemService.get(listName);
        Show show(ioService, listService, listItemService, themeService);
        REQUIRE_NOTHROW(show.print(items, listName, false));
    }

    SECTION("print — showTitle=false does not throw")
    {
        std::vector<ListItemEntity> items = listItemService.get(listName);
        Show show(ioService, listService, listItemService, themeService);
        REQUIRE_NOTHROW(show.print(items, listName, true, false));
    }

    // ---- printMultipleList -----------------------------------------------------

    SECTION("printMultipleList — with items does not throw")
    {
        std::vector<ListItemEntity> items = listItemService.get(listName);
        std::vector<ListName> listNames = { listName };
        Show show(ioService, listService, listItemService, themeService);
        REQUIRE_NOTHROW(show.printMultipleList(items, listNames));
    }

    SECTION("printMultipleList — with empty lists does not throw")
    {
        std::vector<ListItemEntity> items;
        std::vector<ListName> listNames;
        Show show(ioService, listService, listItemService, themeService);
        REQUIRE_NOTHROW(show.printMultipleList(items, listNames));
    }
}
