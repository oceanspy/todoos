#include "../../Helpers/BashStyle.h"
#include "../../Helpers/StringHelpers.h"
#include "../../List/ListItems/ListItemEntity.h"
#include "../../List/ListItems/PriorityService.h"
#include "../../List/ListItems/StatusService.h"
#include "../../Themes/Default/Default.h"
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

static ListItemEntity buildOpenItem(const std::string& id,
                                    const std::string& value,
                                    const std::string& priority,
                                    const std::string& status,
                                    ListName& listName,
                                    time_t createdAt = 1704067200,
                                    time_t dueAt = 0)
{
    PriorityService priorityService;
    StatusService statusService;
    PriorityEntity priorityEntity = priorityService.getPriorityFromName(priority);
    StatusEntity statusEntity     = statusService.getStatusFromName(status);
    return ListItemEntity::set(id, value, priorityEntity, statusEntity, dueAt, 0, createdAt, createdAt, listName);
}

static ListItemEntity buildClosedItem(const std::string& id,
                                      const std::string& value,
                                      ListName& listName,
                                      time_t createdAt  = 1704067200,
                                      time_t updatedAt  = 1704844800)
{
    PriorityService priorityService;
    StatusService statusService;
    PriorityEntity priorityEntity = priorityService.getPriorityFromName("high");
    StatusEntity statusEntity     = statusService.getStatusFromName("completed");
    return ListItemEntity::set(id, value, priorityEntity, statusEntity, 0, updatedAt, createdAt, updatedAt, listName);
}

TEST_CASE("ThemeAbstract non-virtual functions", "[ThemeAbstract]")
{
    IOService ioService("cli");
    ConfService confService(ioService);
    JSONService jsonService(ioService);
    std::unique_ptr<FileDataServiceInterface> fileDataStorageServicePtr =
        std::make_unique<JSONService>(ioService);
    std::unique_ptr<FileDataServiceInterface> fileDataConfigStorageServicePtr =
        std::make_unique<ConfService>(ioService);

    MockInit init(ioService, "_todoos_ThemeAbstractTest");
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

    Default theme(ioService, listService, listItemService, 140, 140);

    // ---- buildId ----------------------------------------------------------------

    SECTION("buildId — open item contains the ID")
    {
        ListItemEntity item = buildOpenItem("aaaa", "test value", "high", "to-do", listName);
        std::string result  = theme.buildId(item);

        REQUIRE(result.find("aaaa") != std::string::npos);
        REQUIRE(static_cast<int>(StringHelpers::countCharsWithoutBashCodes(result)) == 6);
    }

    SECTION("buildId — closed item is grayed out")
    {
        ListItemEntity item = buildClosedItem("bbbb", "test value", listName);
        std::string result  = theme.buildId(item);

        REQUIRE(result.find("bbbb") != std::string::npos);
        REQUIRE(result.find(GRAY) != std::string::npos);
        REQUIRE(static_cast<int>(StringHelpers::countCharsWithoutBashCodes(result)) == 6);
    }

    // ---- buildPriority ----------------------------------------------------------

    SECTION("buildPriority — open item uses priority color, not gray")
    {
        ListItemEntity item = buildOpenItem("aaaa", "test value", "high", "to-do", listName);
        std::string result  = theme.buildPriority(item);

        REQUIRE(result.find(GRAY) == std::string::npos);
        REQUIRE(static_cast<int>(StringHelpers::countCharsWithoutBashCodes(result)) == 4);
    }

    SECTION("buildPriority — closed item is grayed out")
    {
        ListItemEntity item = buildClosedItem("bbbb", "test value", listName);
        std::string result  = theme.buildPriority(item);

        REQUIRE(result.find(GRAY) != std::string::npos);
        REQUIRE(static_cast<int>(StringHelpers::countCharsWithoutBashCodes(result)) == 4);
    }

    // ---- buildStatus ------------------------------------------------------------

    SECTION("buildStatus — padded to STATUS_LENGTH")
    {
        ListItemEntity item = buildOpenItem("aaaa", "test value", "high", "started", listName);
        std::string result  = theme.buildStatus(item);

        REQUIRE(static_cast<int>(StringHelpers::countCharsWithoutBashCodes(result)) == 20);
    }

    // ---- buildValue -------------------------------------------------------------

    SECTION("buildValue — value text present in output")
    {
        ListItemEntity item = buildOpenItem("aaaa", "my task title", "high", "to-do", listName);
        std::string result  = theme.buildValue(item, 0);

        REQUIRE(result.find("my task title") != std::string::npos);
    }

    SECTION("buildValue — long text wraps with newline")
    {
        std::string longValue = "";
        for (int i = 0; i < 20; i++) {
            longValue += "word" + std::to_string(i) + " ";
        }
        ListItemEntity item = buildOpenItem("aaaa", longValue, "high", "to-do", listName);
        std::string result  = theme.buildValue(item, 0);

        REQUIRE(result.find('\n') != std::string::npos);
    }

    // ---- buildDate --------------------------------------------------------------

    SECTION("buildDate — closed item shows elapsed duration")
    {
        ListItemEntity item = buildClosedItem("bbbb", "test", listName, 1704067200, 1704844800);
        std::string result  = theme.buildDate(item);

        REQUIRE(result.find("Elaps.:") != std::string::npos);
        REQUIRE(static_cast<int>(StringHelpers::countCharsWithoutBashCodes(result)) == 25);
    }

    SECTION("buildDate — open item with old created date shows human date")
    {
        // created 2024-01-01 — not today, not yesterday
        ListItemEntity item = buildOpenItem("aaaa", "test", "high", "to-do", listName, 1704067200);
        std::string result  = theme.buildDate(item);

        REQUIRE(result.find("Today") == std::string::npos);
        REQUIRE(result.find("Yesterday") == std::string::npos);
        REQUIRE(result.find("Elaps.:") == std::string::npos);
        REQUIRE(static_cast<int>(StringHelpers::countCharsWithoutBashCodes(result)) == 25);
    }

    SECTION("buildDate — open item with due date far in future shows deadline")
    {
        time_t futureDue = time(nullptr) + 60 * 60 * 24 * 30;  // 30 days from now
        ListItemEntity item = buildOpenItem("aaaa", "test", "high", "to-do", listName, 1704067200, futureDue);
        std::string result  = theme.buildDate(item);

        REQUIRE(result.find("Deadl.:") != std::string::npos);
    }

    SECTION("buildDate — open item with overdue due date shows red background")
    {
        time_t overdue = time(nullptr) - 60 * 60 * 24;  // 1 day ago
        ListItemEntity item = buildOpenItem("aaaa", "test", "high", "to-do", listName, 1704067200, overdue);
        std::string result  = theme.buildDate(item);

        REQUIRE(result.find("Deadl.:") != std::string::npos);
        REQUIRE(result.find(BG_RED) != std::string::npos);
    }

    // ---- I/O smoke tests --------------------------------------------------------

    SECTION("printFullLine — does not throw")
    {
        REQUIRE_NOTHROW(theme.printFullLine());
        REQUIRE_NOTHROW(theme.printFullLine(RED));
    }

    SECTION("printAList — empty list does not throw")
    {
        REQUIRE_NOTHROW(theme.printAList({}));
    }

    SECTION("printAList — list with lines does not throw")
    {
        REQUIRE_NOTHROW(theme.printAList({ "line one", "line two" }));
    }

    SECTION("print — empty item list with showTitle does not throw")
    {
        std::vector<ListItemEntity> emptyItems;
        REQUIRE_NOTHROW(theme.print(listName, emptyItems, false, true));
    }

    SECTION("printMultipleList — empty items does not throw")
    {
        std::vector<ListName> listNames    = { listName };
        std::vector<ListItemEntity> emptyItems;
        REQUIRE_NOTHROW(theme.printMultipleList(listNames, emptyItems));
    }
}
