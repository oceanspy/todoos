#include "../../Helpers/StringHelpers.h"
#include "../../List/ListItems/ListItemEntity.h"
#include "../../List/ListItems/PriorityService.h"
#include "../../List/ListItems/StatusService.h"
#include "../../Themes/ThemeService.h"
#include "../../FileDataStorage/ConfService.h"
#include "../../FileDataStorage/JSONService.h"
#include "../../FileDataStorageRepositories/ConfigRepository.h"
#include "../../FileDataStorageRepositories/ListItemRepository.h"
#include "../../FileDataStorageRepositories/ListRepository.h"
#include "../../Config/ConfigService.h"
#include "../../List/ListItemService.h"
#include "../../List/ListService.h"
#include "../../Events/EventBus.h"
#include "../../Command/Command.h"
#include "../Mock/MockInit.h"
#include "../Mock/MockInstallation.h"
#include <catch2/catch_test_macros.hpp>

static ListItemEntity buildItem(const std::string& value, ListName& listName)
{
    PriorityService priorityService;
    StatusService statusService;
    PriorityEntity priorityEntity = priorityService.getPriorityFromName("high");
    StatusEntity statusEntity     = statusService.getStatusFromName("to-do");
    return ListItemEntity::set("aaaa", value, priorityEntity, statusEntity, 0, 0, 0, 0, listName);
}

TEST_CASE("ThemeService", "[ThemeService]")
{
    IOService ioService("cli");
    ConfService confService(ioService);
    JSONService jsonService(ioService);
    std::unique_ptr<FileDataServiceInterface> fileDataStorageServicePtr =
        std::make_unique<JSONService>(ioService);
    std::unique_ptr<FileDataServiceInterface> fileDataConfigStorageServicePtr =
        std::make_unique<ConfService>(ioService);

    MockInit init(ioService, "_todoos_ThemeServiceTest");
    MockInstallation installation(ioService, jsonService, confService, init);
    installation.wipe();
    installation.make();

    Command command("", {}, {}, "");
    ConfigRepository configRepository(fileDataConfigStorageServicePtr.get(), init.getConfigFilePath());
    ConfigRepository cacheRepository(fileDataConfigStorageServicePtr.get(), init.getCacheFilePath());
    ConfigService configService(ioService, init, configRepository, cacheRepository, command);

    PriorityService priorityService;
    StatusService statusService;
    ListItemRepository listItemRepository(
        configService, fileDataStorageServicePtr.get(), priorityService, statusService);
    ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
    std::unique_ptr<FileDataServiceInterface> jsonFileDataStorageServicePtr =
        std::make_unique<JSONService>(ioService);
    ListRepository listRepository(configService, jsonFileDataStorageServicePtr.get());
    EventBus bus;
    ListService listService(ioService, configService, listRepository, bus);

    ListName listName = listService.createUsedListName();

    // ---- getTheme ---------------------------------------------------------------

    SECTION("getTheme — returns a non-null theme")
    {
        ThemeService service(ioService, configService, listService, listItemService);
        REQUIRE(service.getTheme() != nullptr);
    }

    SECTION("getTheme — returned theme is functional")
    {
        ThemeService service(ioService, configService, listService, listItemService);
        auto theme = service.getTheme();
        REQUIRE_NOTHROW(theme->printListTitleRow());
    }

    // ---- substractConsoleRowLength ----------------------------------------------

    SECTION("substractConsoleRowLength — larger subtraction gives shorter title row")
    {
        // Regardless of terminal width, subtracting more always produces a narrower row
        ThemeService serviceA(ioService, configService, listService, listItemService);
        ThemeService serviceB(ioService, configService, listService, listItemService);
        auto themeSmallSub = serviceA.substractConsoleRowLength(5).getTheme();
        auto themeLargeSub = serviceB.substractConsoleRowLength(20).getTheme();
        std::size_t smallSubLen = StringHelpers::countCharsWithoutBashCodes(themeSmallSub->printListTitleRow());
        std::size_t largeSubLen = StringHelpers::countCharsWithoutBashCodes(themeLargeSub->printListTitleRow());
        REQUIRE(smallSubLen > largeSubLen);
    }

    SECTION("substractConsoleRowLength — zero substract is a no-op")
    {
        ThemeService service(ioService, configService, listService, listItemService);
        auto themeNoSub = service.getTheme();
        auto themeSub0  = service.substractConsoleRowLength(0).getTheme();
        REQUIRE(themeNoSub->printListTitleRow() == themeSub0->printListTitleRow());
    }

    // ---- adaptConsoleRowLengthWithMaxItemValueLength ----------------------------

    SECTION("adaptConsoleRowLengthWithMaxItemValueLength — short items does not throw")
    {
        ThemeService service(ioService, configService, listService, listItemService);
        std::vector<ListItemEntity> items = { buildItem("short title", listName) };
        REQUIRE_NOTHROW(service.adaptConsoleRowLengthWithMaxItemValueLength(items).getTheme());
    }

    SECTION("adaptConsoleRowLengthWithMaxItemValueLength — empty items does not throw")
    {
        ThemeService service(ioService, configService, listService, listItemService);
        REQUIRE_NOTHROW(service.adaptConsoleRowLengthWithMaxItemValueLength({}).getTheme());
    }

    SECTION("adaptConsoleRowLengthWithMaxItemValueLength — long items does not throw")
    {
        ThemeService service(ioService, configService, listService, listItemService);
        std::string longTitle(90, 'x');
        std::vector<ListItemEntity> items = { buildItem(longTitle, listName) };
        REQUIRE_NOTHROW(service.adaptConsoleRowLengthWithMaxItemValueLength(items).getTheme());
    }
}
