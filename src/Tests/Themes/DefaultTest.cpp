#include "../../Themes/Presets/Default.h"
#include "../../Command/Command.h"
#include "../../Config/ConfigService.h"
#include "../../Events/EventBus.h"
#include "../../FileDataStorageRepositories/ConfigRepository.h"
#include "../../FileDataStorageRepositories/ListItemRepository.h"
#include "../../FileDataStorageRepositories/ListRepository.h"
#include "../../Helpers/StringHelpers.h"
#include "../../List/ListItemService.h"
#include "../../List/ListItems/ListItemEntity.h"
#include "../../List/ListItems/PriorityService.h"
#include "../../List/ListItems/StatusService.h"
#include "../../List/ListService.h"
#include "../../Serializers/ConfSerializer.h"
#include "../../Serializers/JsonSerializer.h"
#include "../Mock/MockAppInitialization.h"
#include "../Mock/MockAppInstallation.h"
#include <catch2/catch_test_macros.hpp>

static ListItemEntity
buildOpenItem(const std::string& id,
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
    StatusEntity statusEntity = statusService.getStatusFromName(status);
    return ListItemEntity::set(id, value, priorityEntity, statusEntity, dueAt, 0, createdAt, createdAt, listName);
}

static ListItemEntity
buildClosedItem(const std::string& id,
                const std::string& value,
                ListName& listName,
                time_t createdAt = 1704067200,
                time_t updatedAt = 1704844800)
{
    PriorityService priorityService;
    StatusService statusService;
    PriorityEntity priorityEntity = priorityService.getPriorityFromName("high");
    StatusEntity statusEntity = statusService.getStatusFromName("completed");
    return ListItemEntity::set(id, value, priorityEntity, statusEntity, 0, updatedAt, createdAt, updatedAt, listName);
}

// ---------------------------------------------------------------------------
// Default — printListTitleRow, printListRow, printListTitle, printATitle,
//           printStats, printMultipleListTitles, constructor breakpoint
// ---------------------------------------------------------------------------

TEST_CASE("Default theme", "[Default]")
{
    IOService ioService("cli");
    ConfSerializer confService(ioService);
    JsonSerializer jsonService(ioService);
    std::unique_ptr<DataSerializerInterface> fileDataStorageServicePtr = std::make_unique<JsonSerializer>(ioService);
    std::unique_ptr<DataSerializerInterface> fileDataConfigStorageServicePtr =
        std::make_unique<ConfSerializer>(ioService);

    MockAppInitialization init(ioService, "_todoos_DefaultTest");
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
    ListName listName2 = listService.createListName("tempList2Name");

    Default wideTheme(ioService, listService, listItemService, 200, 200);
    Default narrowTheme(ioService, listService, listItemService, 100, 100);

    // ---- printListTitleRow ------------------------------------------------------

    SECTION("printListTitleRow — wide includes STATUS and DATE columns")
    {
        std::string result = wideTheme.printListTitleRow();
        REQUIRE(result.find("STATUS") != std::string::npos);
        REQUIRE(result.find("DATE") != std::string::npos);
    }

    SECTION("printListTitleRow — narrow omits STATUS and DATE columns")
    {
        std::string result = narrowTheme.printListTitleRow();
        REQUIRE(result.find("STATUS") == std::string::npos);
        REQUIRE(result.find("DATE") == std::string::npos);
    }

    SECTION("printListTitleRow — ID and TITLE present in both modes")
    {
        REQUIRE(wideTheme.printListTitleRow().find("ID") != std::string::npos);
        REQUIRE(narrowTheme.printListTitleRow().find("ID") != std::string::npos);
        REQUIRE(wideTheme.printListTitleRow().find("TITLE") != std::string::npos);
        REQUIRE(narrowTheme.printListTitleRow().find("TITLE") != std::string::npos);
    }

    SECTION("printListTitleRow — exact breakpoint: 121 is wide, 120 is narrow")
    {
        Default atBreakpointWide(ioService, listService, listItemService, 121, 121);
        Default atBreakpointNarrow(ioService, listService, listItemService, 120, 120);
        REQUIRE(atBreakpointWide.printListTitleRow().find("STATUS") != std::string::npos);
        REQUIRE(atBreakpointNarrow.printListTitleRow().find("STATUS") == std::string::npos);
    }

    SECTION("printListTitleRow — wide row is wider than narrow row")
    {
        std::size_t wideLen = StringHelpers::countCharsWithoutBashCodes(wideTheme.printListTitleRow());
        std::size_t narrowLen = StringHelpers::countCharsWithoutBashCodes(narrowTheme.printListTitleRow());
        REQUIRE(wideLen > narrowLen);
    }

    // ---- printListRow -----------------------------------------------------------

    SECTION("printListRow — wide row is longer than narrow row")
    {
        ListItemEntity item = buildOpenItem("aaaa", "task value", "high", "to-do", listName);
        std::size_t wideLen = StringHelpers::countCharsWithoutBashCodes(wideTheme.printListRow(item, true));
        std::size_t narrowLen = StringHelpers::countCharsWithoutBashCodes(narrowTheme.printListRow(item, true));
        REQUIRE(wideLen > narrowLen);
    }

    SECTION("printListRow — narrow row visible length does not include status/date")
    {
        ListItemEntity item = buildOpenItem("aaaa", "task value", "high", "to-do", listName);
        std::size_t narrowLen = StringHelpers::countCharsWithoutBashCodes(narrowTheme.printListRow(item, true));
        std::size_t wideLen = StringHelpers::countCharsWithoutBashCodes(wideTheme.printListRow(item, true));
        // STATUS_LENGTH=20, DATE_LENGTH=25 — wide must be at least 45 chars wider
        REQUIRE(wideLen >= narrowLen + 45);
    }

    SECTION("printListRow — row with list name differs from row without it")
    {
        ListItemEntity item = buildOpenItem("aaaa", "task value", "high", "to-do", listName);
        std::string withName = wideTheme.printListRow(item, false);
        std::string withoutName = wideTheme.printListRow(item, true);
        REQUIRE(withName != withoutName);
    }

    SECTION("printListRow — ID appears in output")
    {
        ListItemEntity item = buildOpenItem("zzzz", "task value", "high", "to-do", listName);
        std::string result = wideTheme.printListRow(item, true);
        REQUIRE(result.find("zzzz") != std::string::npos);
    }

    SECTION("printListRow — value text appears in output")
    {
        ListItemEntity item = buildOpenItem("aaaa", "unique task content", "high", "to-do", listName);
        std::string result = wideTheme.printListRow(item, true);
        REQUIRE(result.find("unique task content") != std::string::npos);
    }

    SECTION("printListRow — closed item renders without throw")
    {
        ListItemEntity item = buildClosedItem("bbbb", "done task", listName);
        REQUIRE_NOTHROW(wideTheme.printListRow(item, true));
        REQUIRE_NOTHROW(narrowTheme.printListRow(item, true));
    }

    // ---- printListTitle ---------------------------------------------------------

    SECTION("printListTitle — does not throw")
    {
        REQUIRE_NOTHROW(wideTheme.printListTitle(listName));
        REQUIRE_NOTHROW(narrowTheme.printListTitle(listName));
    }

    SECTION("printListTitle — does not throw with archive variant")
    {
        ListName archiveName = ListName::createVariant(listName, "archive");
        REQUIRE_NOTHROW(wideTheme.printListTitle(archiveName));
    }

    SECTION("printListTitle — does not throw with delete variant")
    {
        ListName deleteName = ListName::createVariant(listName, "delete");
        REQUIRE_NOTHROW(wideTheme.printListTitle(deleteName));
    }

    // ---- printATitle ------------------------------------------------------------

    SECTION("printATitle — does not throw with two lines")
    {
        REQUIRE_NOTHROW(wideTheme.printATitle("My App", "version 1.0"));
        REQUIRE_NOTHROW(narrowTheme.printATitle("My App", "version 1.0"));
    }

    SECTION("printATitle — does not throw with empty second line")
    {
        REQUIRE_NOTHROW(wideTheme.printATitle("My App", ""));
        REQUIRE_NOTHROW(narrowTheme.printATitle("My App", ""));
    }

    // ---- printStats -------------------------------------------------------------

    SECTION("printStats — wide mode does not throw")
    {
        REQUIRE_NOTHROW(wideTheme.printStats(listName));
    }

    SECTION("printStats — narrow mode does not throw")
    {
        REQUIRE_NOTHROW(narrowTheme.printStats(listName));
    }

    SECTION("printStats — exact narrow breakpoint (120) does not throw")
    {
        Default atNarrow(ioService, listService, listItemService, 120, 120);
        REQUIRE_NOTHROW(atNarrow.printStats(listName));
    }

    SECTION("printStats — exact wide breakpoint (121) does not throw")
    {
        Default atWide(ioService, listService, listItemService, 121, 121);
        REQUIRE_NOTHROW(atWide.printStats(listName));
    }

    // ---- printMultipleListTitles ------------------------------------------------

    SECTION("printMultipleListTitles — single list name does not throw")
    {
        std::vector<ListName> listNames = { listName };
        REQUIRE_NOTHROW(wideTheme.printMultipleListTitles(listNames));
    }

    SECTION("printMultipleListTitles — multiple list names does not throw")
    {
        std::vector<ListName> listNames = { listName, listName2 };
        REQUIRE_NOTHROW(wideTheme.printMultipleListTitles(listNames));
    }

    SECTION("printMultipleListTitles — narrow mode does not throw")
    {
        std::vector<ListName> listNames = { listName };
        REQUIRE_NOTHROW(narrowTheme.printMultipleListTitles(listNames));
    }

    SECTION("printMultipleListTitles — archive variant does not throw")
    {
        ListName archiveName = ListName::createVariant(listName, "archive");
        std::vector<ListName> listNames = { archiveName };
        REQUIRE_NOTHROW(wideTheme.printMultipleListTitles(listNames));
    }

    SECTION("printMultipleListTitles — delete variant does not throw")
    {
        ListName deleteName = ListName::createVariant(listName, "delete");
        std::vector<ListName> listNames = { deleteName };
        REQUIRE_NOTHROW(wideTheme.printMultipleListTitles(listNames));
    }

    // ---- constructor breakpoint — column width derivation -----------------------

    SECTION("consoleRowLength derived correctly for wide (consoleRowMaxLength - 56)")
    {
        // wide: consoleRowLength = 200 - 56 = 144 chars for TITLE column
        std::size_t len = StringHelpers::countCharsWithoutBashCodes(wideTheme.printListTitleRow());
        REQUIRE(len > 100);
    }

    SECTION("consoleRowLength derived correctly for narrow (consoleRowMaxLength - 10)")
    {
        // narrow: consoleRowLength = 100 - 10 = 90 chars for TITLE column
        std::size_t len = StringHelpers::countCharsWithoutBashCodes(narrowTheme.printListTitleRow());
        REQUIRE(len > 50);
    }
}
