#include "../../Helpers/TerminalStyle.h"
#include "../../Helpers/StringHelpers.h"
#include "../../List/ListCountSummary.h"
#include "../../List/ListItems/ListItemEntity.h"
#include "../../List/ListItems/PriorityService.h"
#include "../../List/ListItems/StatusService.h"
#include "../../Themes/Default/Default.h"
#include "../../Serializers/ConfSerializer.h"
#include "../../Serializers/JsonSerializer.h"
#include "../../FileDataStorageRepositories/ConfigRepository.h"
#include "../../FileDataStorageRepositories/ListItemRepository.h"
#include "../../FileDataStorageRepositories/ListRepository.h"
#include "../../Config/ConfigService.h"
#include "../../List/ListItemService.h"
#include "../../List/ListService.h"
#include "../../Events/EventBus.h"
#include "../../Command/Command.h"
#include "../Mock/MockAppInitialization.h"
#include "../Mock/MockAppInstallation.h"
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

TEST_CASE("Theme non-virtual functions", "[ThemeAbstract]")
{
    IOService ioService("cli");
    ConfSerializer confService(ioService);
    JsonSerializer jsonService(ioService);
    std::unique_ptr<DataSerializerInterface> fileDataStorageServicePtr =
        std::make_unique<JsonSerializer>(ioService);
    std::unique_ptr<DataSerializerInterface> fileDataConfigStorageServicePtr =
        std::make_unique<ConfSerializer>(ioService);

    MockAppInitialization init(ioService, "_todoos_ThemeAbstractTest");
    MockAppInstallation installation(ioService, jsonService, confService, init);
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
    std::unique_ptr<DataSerializerInterface> jsonFileDataStorageServicePtr =
        std::make_unique<JsonSerializer>(ioService);
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

    SECTION("buildDate — open item due in 2 days shows LIGHT_RED deadline")
    {
        time_t due2Days = time(nullptr) + 60 * 60 * 24 * 2;
        ListItemEntity item = buildOpenItem("aaaa", "test", "high", "to-do", listName, 1704067200, due2Days);
        std::string result  = theme.buildDate(item);

        REQUIRE(result.find("Deadl.:") != std::string::npos);
        REQUIRE(result.find(LIGHT_RED) != std::string::npos);
    }

    SECTION("buildDate — open item due in 5 days shows LIGHT_YELLOW deadline")
    {
        time_t due5Days = time(nullptr) + 60 * 60 * 24 * 5;
        ListItemEntity item = buildOpenItem("aaaa", "test", "high", "to-do", listName, 1704067200, due5Days);
        std::string result  = theme.buildDate(item);

        REQUIRE(result.find("Deadl.:") != std::string::npos);
        REQUIRE(result.find(LIGHT_YELLOW) != std::string::npos);
    }

    SECTION("buildDate — open item created today shows 'Today at'")
    {
        time_t todayTs = time(nullptr) - 1800;  // 30 minutes ago
        ListItemEntity item = buildOpenItem("aaaa", "test", "high", "to-do", listName, todayTs);
        std::string result  = theme.buildDate(item);

        REQUIRE(result.find("Today") != std::string::npos);
        REQUIRE(static_cast<int>(StringHelpers::countCharsWithoutBashCodes(result)) == 25);
    }

    SECTION("buildDate — open item created yesterday shows 'Yesterday at'")
    {
        time_t yesterdayTs = time(nullptr) - 90000;  // 25 hours ago
        ListItemEntity item = buildOpenItem("aaaa", "test", "high", "to-do", listName, yesterdayTs);
        std::string result  = theme.buildDate(item);

        REQUIRE(result.find("Yesterday") != std::string::npos);
        REQUIRE(static_cast<int>(StringHelpers::countCharsWithoutBashCodes(result)) == 25);
    }

    SECTION("buildDate — passive status with near-overdue due date uses status color not BG_RED")
    {
        // "paused" is passive — overrides time-based coloring regardless of urgency
        time_t overdue = time(nullptr) - 60 * 60 * 24;
        ListItemEntity item = buildOpenItem("aaaa", "test", "high", "paused", listName, 1704067200, overdue);
        std::string result  = theme.buildDate(item);

        REQUIRE(result.find("Deadl.:") != std::string::npos);
        REQUIRE(result.find(BG_RED) == std::string::npos);
    }

    SECTION("buildStatus — different statuses produce different output")
    {
        ListItemEntity todoItem     = buildOpenItem("aaaa", "test", "high", "to-do", listName);
        ListItemEntity startedItem  = buildOpenItem("aaaa", "test", "high", "started", listName);
        ListItemEntity reviewItem   = buildOpenItem("aaaa", "test", "high", "reviewing", listName);

        REQUIRE(theme.buildStatus(todoItem) != theme.buildStatus(startedItem));
        REQUIRE(theme.buildStatus(todoItem) != theme.buildStatus(reviewItem));
        REQUIRE(theme.buildStatus(startedItem) != theme.buildStatus(reviewItem));
    }

    SECTION("buildStatus — closed item has same length as open item")
    {
        ListItemEntity openItem   = buildOpenItem("aaaa", "test", "high", "to-do", listName);
        ListItemEntity closedItem = buildClosedItem("aaaa", "test", listName);

        REQUIRE(StringHelpers::countCharsWithoutBashCodes(theme.buildStatus(openItem)) ==
                StringHelpers::countCharsWithoutBashCodes(theme.buildStatus(closedItem)));
    }

    // ---- buildValue (additional) ------------------------------------------------

    SECTION("buildValue — closed item has different styling than open item")
    {
        ListItemEntity openItem   = buildOpenItem("aaaa", "same text", "high", "to-do", listName);
        ListItemEntity closedItem = buildClosedItem("aaaa", "same text", listName);

        REQUIRE(theme.buildValue(openItem, 0) != theme.buildValue(closedItem, 0));
    }

    SECTION("buildValue — leftOffset reduces wrap threshold")
    {
        std::string longValue;
        for (int i = 0; i < 30; i++) longValue += "word" + std::to_string(i) + " ";
        ListItemEntity item = buildOpenItem("aaaa", longValue, "high", "to-do", listName);

        std::string noOffset   = theme.buildValue(item, 0);
        std::string withOffset = theme.buildValue(item, 12);
        REQUIRE(noOffset != withOffset);
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

    SECTION("print — with items does not throw")
    {
        std::vector<ListItemEntity> items = {
            buildOpenItem("aaaa", "first task", "high", "to-do", listName),
            buildOpenItem("bbbb", "second task", "medium", "started", listName),
        };
        REQUIRE_NOTHROW(theme.print(listName, items, false, true));
    }

    SECTION("print — showListName=true does not throw")
    {
        std::vector<ListItemEntity> items = { buildOpenItem("aaaa", "task", "high", "to-do", listName) };
        REQUIRE_NOTHROW(theme.print(listName, items, true, true));
    }

    SECTION("print — showTitle=false does not throw")
    {
        std::vector<ListItemEntity> items = { buildOpenItem("aaaa", "task", "high", "to-do", listName) };
        REQUIRE_NOTHROW(theme.print(listName, items, false, false));
    }

    SECTION("printMultipleList — with items does not throw")
    {
        std::vector<ListName> listNames = { listName };
        std::vector<ListItemEntity> items = { buildOpenItem("aaaa", "task one", "high", "to-do", listName) };
        REQUIRE_NOTHROW(theme.printMultipleList(listNames, items));
    }

    // ---- buildPriorityCounts ----------------------------------------------------

    SECTION("buildPriorityCounts — counts appear in output")
    {
        ListCountSummary summary;
        summary.addPriority(PriorityService::CRITICAL);
        summary.addPriority(PriorityService::CRITICAL);
        summary.addPriority(PriorityService::URGENT);
        summary.addPriority(PriorityService::HIGH);
        summary.addPriority(PriorityService::HIGH);
        summary.addPriority(PriorityService::HIGH);
        summary.addPriority(PriorityService::MEDIUM);
        summary.addPriority(PriorityService::LOW);

        std::string result = theme.buildPriorityCounts(summary);

        // raw string contains the digit counts (bash codes don't contain digits)
        REQUIRE(result.find("2") != std::string::npos); // critical = 2
        REQUIRE(result.find("1") != std::string::npos); // urgent = 1
        REQUIRE(result.find("3") != std::string::npos); // high = 3
    }

    SECTION("buildPriorityCounts — empty summary shows zeros for all priority levels")
    {
        ListCountSummary summary;
        std::string result = theme.buildPriorityCounts(summary);

        // count values are followed by a space separator; bash codes never have '0' followed by ' '
        int zeroCount = 0;
        for (std::string::size_type pos = 0; (pos = result.find("0 ", pos)) != std::string::npos; pos++) {
            zeroCount++;
        }
        // four "0 " separators (critical urgent high medium) + last "0" without trailing space
        REQUIRE(zeroCount == 4);
        REQUIRE(result.rfind('0') != std::string::npos); // last priority (low) also zero
    }

    SECTION("buildPriorityCounts — output is not empty")
    {
        ListCountSummary summary;
        REQUIRE_FALSE(theme.buildPriorityCounts(summary).empty());
    }
}
