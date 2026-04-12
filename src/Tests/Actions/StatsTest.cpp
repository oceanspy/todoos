#include "../../Actions/StatsAction/StatsAction.h"
#include "../../Config/ConfigService.h"
#include "../../Events/EventBus.h"
#include "../../Serializers/ConfSerializer.h"
#include "../../Serializers/JsonSerializer.h"
#include "../../FileDataStorageRepositories/ConfigRepository.h"
#include "../../FileDataStorageRepositories/ListItemRepository.h"
#include "../../FileDataStorageRepositories/ListRepository.h"
#include "../../List/ListItemService.h"
#include "../../List/ListItems/PriorityService.h"
#include "../../List/ListItems/StatusService.h"
#include "../../List/ListService.h"
#include "../../Themes/ThemeService.h"
#include "../Mock/MockAppInitialization.h"
#include "../Mock/MockAppInstallation.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Stats action", "[Stats]")
{
    IOService ioService("cli");
    ConfSerializer confService(ioService);
    JsonSerializer jsonService(ioService);
    std::unique_ptr<DataSerializerInterface> storagePtr = std::make_unique<JsonSerializer>(ioService);
    std::unique_ptr<DataSerializerInterface> configStoragePtr = std::make_unique<ConfSerializer>(ioService);

    MockAppInitialization init(ioService, "_todoos_StatsActionTest");
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
    std::unique_ptr<DataSerializerInterface> listStoragePtr = std::make_unique<JsonSerializer>(ioService);
    ListRepository listRepository(configService, listStoragePtr.get());
    ListService listService(ioService, configService, listRepository, bus);
    ThemeService themeService(ioService, configService, listService, listItemService);

    ListName listName = listService.createUsedListName();

    // ---- print -----------------------------------------------------------------

    SECTION("print — with items does not throw")
    {
        StatsAction stats(ioService, configService, command, listItemService, themeService, listName);
        REQUIRE_NOTHROW(stats.print());
    }

    SECTION("print — archive list variant does not throw")
    {
        ListName archiveName = ListName::createVariant(listName, "archive");
        StatsAction stats(ioService, configService, command, listItemService, themeService, archiveName);
        REQUIRE_NOTHROW(stats.print());
    }
}
