#include "../../Actions/ShowAction/ShowAction.h"
#include "../../Config/ConfigService.h"
#include "../../Events/EventBus.h"
#include "../../FileDataStorageRepositories/ConfigRepository.h"
#include "../../FileDataStorageRepositories/ListItemRepository.h"
#include "../../FileDataStorageRepositories/ListRepository.h"
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

TEST_CASE("Show action", "[Show]")
{
    IOService ioService("cli");
    ConfSerializer confService(ioService);
    JsonSerializer jsonService(ioService);
    std::unique_ptr<DataSerializerInterface> storagePtr = std::make_unique<JsonSerializer>(ioService);
    std::unique_ptr<DataSerializerInterface> configStoragePtr = std::make_unique<ConfSerializer>(ioService);

    MockAppInitialization init(ioService, "_todoos_ShowActionTest");
    MockAppInstallation installation(ioService, jsonService, confService, init);
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
    std::unique_ptr<DataSerializerInterface> listStoragePtr = std::make_unique<JsonSerializer>(ioService);
    ListRepository listRepository(configService, listStoragePtr.get());
    ListService listService(ioService, configService, listRepository, bus);
    ThemeService themeService(ioService, configService, listService, listItemService);

    ListName listName = listService.createUsedListName();

    // ---- print -----------------------------------------------------------------

    SECTION("print — with populated items does not throw")
    {
        std::vector<ListItemEntity> items = listItemService.get(listName);
        ShowAction show(ioService, listService, listItemService, themeService);
        REQUIRE_NOTHROW(show.execute(items, listName));
    }

    SECTION("print — with empty items does not throw")
    {
        std::vector<ListItemEntity> items;
        ShowAction show(ioService, listService, listItemService, themeService);
        REQUIRE_NOTHROW(show.execute(items, listName));
    }

    SECTION("print — showListName=false does not throw")
    {
        std::vector<ListItemEntity> items = listItemService.get(listName);
        ShowAction show(ioService, listService, listItemService, themeService);
        REQUIRE_NOTHROW(show.execute(items, listName, false));
    }

    SECTION("print — showTitle=false does not throw")
    {
        std::vector<ListItemEntity> items = listItemService.get(listName);
        ShowAction show(ioService, listService, listItemService, themeService);
        REQUIRE_NOTHROW(show.execute(items, listName, true, false));
    }

    // ---- printMultipleList -----------------------------------------------------

    SECTION("printMultipleList — with items does not throw")
    {
        std::vector<ListItemEntity> items = listItemService.get(listName);
        std::vector<ListName> listNames = { listName };
        ShowAction show(ioService, listService, listItemService, themeService);
        REQUIRE_NOTHROW(show.executeMultipleList(items, listNames));
    }

    SECTION("printMultipleList — with empty lists does not throw")
    {
        std::vector<ListItemEntity> items;
        std::vector<ListName> listNames;
        ShowAction show(ioService, listService, listItemService, themeService);
        REQUIRE_THROWS(show.executeMultipleList(items, listNames));
    }
}
