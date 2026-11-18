#include "../../Command/Command.h"
#include "../../Config/ConfigService.h"
#include "../../Events/EventBus.h"
#include "../../FileDataStorageRepositories/ConfigRepository.h"
#include "../../FileDataStorageRepositories/ListItemRepository.h"
#include "../../FileDataStorageRepositories/ListRepository.h"
#include "../../Helpers/StringHelpers.h"
#include "../../Helpers/TerminalStyle.h"
#include "../../List/ListCountSummary.h"
#include "../../FileDataStorageRepositories/DescriptionRepository.h"
#include "../../List/ListItemService.h"
#include "../../List/ListItems/ListItemEntity.h"
#include "../../List/ListItems/PriorityService.h"
#include "../../List/ListItems/StatusService.h"
#include "../../List/ListService.h"
#include "../../Serializers/ConfSerializer.h"
#include "../../Serializers/JsonSerializer.h"
#include "../../Themes/Presets/Default.h"
#include "../Mock/MockAppInitialization.h"
#include "../Mock/MockAppInstallation.h"
#include <catch2/catch_test_macros.hpp>
#include <regex>
#include <sstream>

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

// Default theme that prints a row as a child row, with the connector print() would give it
class ChildRowTheme : public Default
{
  public:
    using Default::Default;
    std::string childRow(const ListItemEntity& item, const std::string& connector)
    {
        childConnector = connector;
        std::string row = printListRow(item, true);
        childConnector = "";
        return row;
    }
};

// Column (in characters) of the first character that is neither a space nor the `│` connector
static long
textColumn(const std::string& line)
{
    long column = 0;
    for (size_t i = 0; i < line.size(); i++) {
        if ((static_cast<unsigned char>(line[i]) & 0xC0) == 0x80) {
            continue;
        }
        if (line[i] != ' ' && line.compare(i, std::string("│").size(), "│") != 0) {
            return column;
        }
        column++;
    }
    return -1;
}

// Visible lines of a rendered row (color codes removed)
static std::vector<std::string>
visibleLines(const std::string& row)
{
    static const std::regex colorCodes("\033\\[[0-9;]*m");
    std::vector<std::string> lines;
    std::stringstream stream(std::regex_replace(row, colorCodes, ""));
    for (std::string line; std::getline(stream, line);) {
        lines.push_back(line);
    }
    return lines;
}

// Column (in characters, not bytes) of `text` in a UTF-8 line
static long
columnOf(const std::string& line, const std::string& text)
{
    size_t at = line.find(text);
    if (at == std::string::npos) {
        return -1;
    }
    long column = 0;
    for (size_t i = 0; i < at; i++) {
        if ((static_cast<unsigned char>(line[i]) & 0xC0) != 0x80) {
            column++;
        }
    }
    return column;
}

TEST_CASE("Theme non-virtual functions", "[ThemeAbstract]")
{
    IOService ioService("cli");
    ConfSerializer confService(ioService);
    JsonSerializer jsonService(ioService);
    std::unique_ptr<DataSerializerInterface> fileDataStorageServicePtr = std::make_unique<JsonSerializer>(ioService);
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
    DescriptionRepository descriptionRepository(configService.getDescriptionsDirPath());

    ListItemService listItemService(ioService, configService, listItemRepository, descriptionRepository, priorityService, statusService);
    std::unique_ptr<DataSerializerInterface> jsonFileDataStorageServicePtr =
        std::make_unique<JsonSerializer>(ioService);
    ListRepository listRepository(configService, jsonFileDataStorageServicePtr.get());
    EventBus bus;
    ListService listService(ioService, configService, listRepository, bus);

    ListName listName = listService.createUsedListName();

    Default theme(ioService, listService, listItemService, 140, 140);

    // ---- parent / child rows ------------------------------------------------------

    SECTION("groupChildren puts children right after their parent; children of a hidden parent stay top-level")
    {
        ListName otherList = listService.createListName("tempList2Name");
        ListItemEntity parent = buildOpenItem("pppp", "parent", "high", "queued", listName);
        ListItemEntity child1 = buildOpenItem("c111", "child 1", "high", "queued", listName);
        ListItemEntity child2 = buildOpenItem("c222", "child 2", "low", "queued", listName);
        ListItemEntity other = buildOpenItem("oooo", "other", "medium", "queued", listName);
        ListItemEntity orphan = buildOpenItem("c333", "orphan", "low", "queued", listName);
        // same parent id, but in another list: not a child of pppp here
        ListItemEntity otherListItem = buildOpenItem("c444", "other list", "low", "queued", otherList);
        child1.setParentId("pppp");
        child2.setParentId("pppp");
        orphan.setParentId("gone");
        otherListItem.setParentId("pppp");

        std::vector<ListItemEntity> rows = { child1, other, parent, orphan, otherListItem, child2 };
        ListItemService::groupChildren(rows);

        std::vector<std::string> ids;
        for (const ListItemEntity& row : rows) {
            ids.push_back(*row.getId());
        }
        REQUIRE(ids == std::vector<std::string>{ "oooo", "pppp", "c111", "c222", "c333", "c444" });

        REQUIRE(Theme::childConnectorFor(rows, 0).empty());
        REQUIRE(Theme::childConnectorFor(rows, 1).empty());
        REQUIRE(Theme::childConnectorFor(rows, 2) == "├─ ");
        REQUIRE(Theme::childConnectorFor(rows, 3) == "└─ ");
        REQUIRE(Theme::childConnectorFor(rows, 4).empty());
        REQUIRE(Theme::childConnectorFor(rows, 5).empty());
    }

    SECTION("a child of a child is shown top-level (one level only)")
    {
        ListItemEntity parent = buildOpenItem("pppp", "parent", "high", "queued", listName);
        ListItemEntity child = buildOpenItem("c111", "child", "high", "queued", listName);
        ListItemEntity grandChild = buildOpenItem("c222", "grand child", "high", "queued", listName);
        child.setParentId("pppp");
        grandChild.setParentId("c111");

        std::vector<ListItemEntity> rows = { grandChild, child, parent };
        ListItemService::groupChildren(rows);
        REQUIRE(rows.size() == 3);
        REQUIRE(*rows[0].getId() == "c222");
        REQUIRE(Theme::childConnectorFor(rows, 0).empty());
        REQUIRE(Theme::childConnectorFor(rows, 2) == "└─ ");
        REQUIRE_NOTHROW(theme.print(listName, rows, false, true));
    }

    // ---- buildId ----------------------------------------------------------------

    SECTION("fitIdColumn — 5-char ids widen the ID column, TITLE shrinks so the line keeps its length")
    {
        ListName archiveName = ListName::createVariant(listName, "archive");
        ListItemEntity activeItem = buildOpenItem("aaaa", "test value", "high", "queued", listName);
        ListItemEntity archivedItem = buildOpenItem("aaaaq", "test value", "high", "queued", archiveName);

        theme.fitIdColumn({ activeItem });
        std::string activeHeader = theme.printListTitleRow();
        std::string activeRow = theme.printListRow(activeItem, true);
        REQUIRE(static_cast<int>(StringHelpers::countCharsWithoutBashCodes(theme.buildId(activeItem))) == 6);

        theme.fitIdColumn({ archivedItem });
        std::string archivedHeader = theme.printListTitleRow();
        std::string archivedRow = theme.printListRow(archivedItem, true);
        REQUIRE(static_cast<int>(StringHelpers::countCharsWithoutBashCodes(theme.buildId(archivedItem))) == 7);
        REQUIRE(theme.buildId(archivedItem).find("aaaaq") != std::string::npos);

        REQUIRE(StringHelpers::countCharsWithoutBashCodes(archivedHeader) ==
                StringHelpers::countCharsWithoutBashCodes(activeHeader));
        REQUIRE(StringHelpers::countCharsWithoutBashCodes(archivedRow) ==
                StringHelpers::countCharsWithoutBashCodes(activeRow));
        // TITLE column starts one char further right
        REQUIRE(archivedHeader.find("TITLE") == activeHeader.find("TITLE") + 1);
    }

    SECTION("buildId — open item contains the ID")
    {
        ListItemEntity item = buildOpenItem("aaaa", "test value", "high", "queued", listName);
        std::string result = theme.buildId(item);

        REQUIRE(result.find("aaaa") != std::string::npos);
        REQUIRE(static_cast<int>(StringHelpers::countCharsWithoutBashCodes(result)) == 6);
    }

    SECTION("buildId — closed item is grayed out")
    {
        ListItemEntity item = buildClosedItem("bbbb", "test value", listName);
        std::string result = theme.buildId(item);

        REQUIRE(result.find("bbbb") != std::string::npos);
        REQUIRE(result.find(GRAY) != std::string::npos);
        REQUIRE(static_cast<int>(StringHelpers::countCharsWithoutBashCodes(result)) == 6);
    }

    // ---- buildPriority ----------------------------------------------------------

    SECTION("buildPriority — open item uses priority color, not gray")
    {
        ListItemEntity item = buildOpenItem("aaaa", "test value", "high", "queued", listName);
        std::string result = theme.buildPriority(item);

        REQUIRE(result.find(GRAY) == std::string::npos);
        REQUIRE(static_cast<int>(StringHelpers::countCharsWithoutBashCodes(result)) == 4);
    }

    SECTION("buildPriority — closed item is grayed out")
    {
        ListItemEntity item = buildClosedItem("bbbb", "test value", listName);
        std::string result = theme.buildPriority(item);

        REQUIRE(result.find(GRAY) != std::string::npos);
        REQUIRE(static_cast<int>(StringHelpers::countCharsWithoutBashCodes(result)) == 4);
    }

    // ---- buildStatus ------------------------------------------------------------

    SECTION("buildStatus — padded to STATUS_LENGTH")
    {
        ListItemEntity item = buildOpenItem("aaaa", "test value", "high", "started", listName);
        std::string result = theme.buildStatus(item);

        REQUIRE(static_cast<int>(StringHelpers::countCharsWithoutBashCodes(result)) == 20);
    }

    // ---- buildValue -------------------------------------------------------------

    SECTION("buildValue — value text present in output")
    {
        ListItemEntity item = buildOpenItem("aaaa", "my task title", "high", "queued", listName);
        std::string result = theme.buildValue(item, 0);

        REQUIRE(result.find("my task title") != std::string::npos);
    }

    SECTION("buildValue — long text wraps with newline")
    {
        std::string longValue = "";
        for (int i = 0; i < 20; i++) {
            longValue += "word" + std::to_string(i) + " ";
        }
        ListItemEntity item = buildOpenItem("aaaa", longValue, "high", "queued", listName);
        std::string result = theme.buildValue(item, 0);

        REQUIRE(result.find('\n') != std::string::npos);
    }

    // ---- buildDate --------------------------------------------------------------

    SECTION("buildDate — closed item shows elapsed duration")
    {
        ListItemEntity item = buildClosedItem("bbbb", "test", listName, 1704067200, 1704844800);
        std::string result = theme.buildDate(item);

        REQUIRE(result.find("Elaps.:") != std::string::npos);
        REQUIRE(static_cast<int>(StringHelpers::countCharsWithoutBashCodes(result)) == 25);
    }

    SECTION("buildDate — open item with old created date shows human date")
    {
        // created 2024-01-01 — not today, not yesterday
        ListItemEntity item = buildOpenItem("aaaa", "test", "high", "queued", listName, 1704067200);
        std::string result = theme.buildDate(item);

        REQUIRE(result.find("Today") == std::string::npos);
        REQUIRE(result.find("Yesterday") == std::string::npos);
        REQUIRE(result.find("Elaps.:") == std::string::npos);
        REQUIRE(static_cast<int>(StringHelpers::countCharsWithoutBashCodes(result)) == 25);
    }

    SECTION("buildDate — open item with due date far in future shows deadline")
    {
        time_t futureDue = time(nullptr) + 60 * 60 * 24 * 30; // 30 days from now
        ListItemEntity item = buildOpenItem("aaaa", "test", "high", "queued", listName, 1704067200, futureDue);
        std::string result = theme.buildDate(item);

        REQUIRE(result.find("Deadl.:") != std::string::npos);
    }

    SECTION("buildDate — open item with overdue due date shows red background")
    {
        time_t overdue = time(nullptr) - 60 * 60 * 24; // 1 day ago
        ListItemEntity item = buildOpenItem("aaaa", "test", "high", "queued", listName, 1704067200, overdue);
        std::string result = theme.buildDate(item);

        REQUIRE(result.find("Deadl.:") != std::string::npos);
        REQUIRE(result.find(BG_RED) != std::string::npos);
    }

    SECTION("buildDate — open item due in 2 days shows LIGHT_RED deadline")
    {
        time_t due2Days = time(nullptr) + 60 * 60 * 24 * 2;
        ListItemEntity item = buildOpenItem("aaaa", "test", "high", "queued", listName, 1704067200, due2Days);
        std::string result = theme.buildDate(item);

        REQUIRE(result.find("Deadl.:") != std::string::npos);
        REQUIRE(result.find(LIGHT_RED) != std::string::npos);
    }

    SECTION("buildDate — open item due in 5 days shows LIGHT_YELLOW deadline")
    {
        time_t due5Days = time(nullptr) + 60 * 60 * 24 * 5;
        ListItemEntity item = buildOpenItem("aaaa", "test", "high", "queued", listName, 1704067200, due5Days);
        std::string result = theme.buildDate(item);

        REQUIRE(result.find("Deadl.:") != std::string::npos);
        REQUIRE(result.find(LIGHT_YELLOW) != std::string::npos);
    }

    SECTION("buildDate — open item due in 11 days shows GREEN deadline")
    {
        time_t due11Days = time(nullptr) + 60 * 60 * 24 * 11;
        ListItemEntity item = buildOpenItem("aaaa", "test", "high", "queued", listName, 1704067200, due11Days);
        std::string result = theme.buildDate(item);

        REQUIRE(result.find("Deadl.:") != std::string::npos);
        REQUIRE(result.find(GREEN) != std::string::npos);
    }

    SECTION("buildDate — open item created today shows 'Today at'")
    {
        time_t todayTs = time(nullptr) - 1800; // 30 minutes ago
        ListItemEntity item = buildOpenItem("aaaa", "test", "high", "queued", listName, todayTs);
        std::string result = theme.buildDate(item);

        REQUIRE(result.find("Today") != std::string::npos);
        REQUIRE(static_cast<int>(StringHelpers::countCharsWithoutBashCodes(result)) == 25);
    }

    SECTION("buildDate — open item created yesterday shows 'Yesterday at'")
    {
        time_t yesterdayTs = time(nullptr) - 90000; // 25 hours ago
        ListItemEntity item = buildOpenItem("aaaa", "test", "high", "queued", listName, yesterdayTs);
        std::string result = theme.buildDate(item);

        REQUIRE(result.find("Yesterday") != std::string::npos);
        REQUIRE(static_cast<int>(StringHelpers::countCharsWithoutBashCodes(result)) == 25);
    }

    SECTION("buildDate — passive status with near-overdue due date uses status color not BG_RED")
    {
        // "paused" is passive — overrides time-based coloring regardless of urgency
        time_t overdue = time(nullptr) - 60 * 60 * 24;
        ListItemEntity item = buildOpenItem("aaaa", "test", "high", "paused", listName, 1704067200, overdue);
        std::string result = theme.buildDate(item);

        REQUIRE(result.find("Deadl.:") != std::string::npos);
        REQUIRE(result.find(BG_RED) == std::string::npos);
    }

    SECTION("buildStatus — different statuses produce different output")
    {
        ListItemEntity todoItem = buildOpenItem("aaaa", "test", "high", "queued", listName);
        ListItemEntity startedItem = buildOpenItem("aaaa", "test", "high", "started", listName);
        ListItemEntity reviewItem = buildOpenItem("aaaa", "test", "high", "triaged", listName);

        REQUIRE(theme.buildStatus(todoItem) != theme.buildStatus(startedItem));
        REQUIRE(theme.buildStatus(todoItem) != theme.buildStatus(reviewItem));
        REQUIRE(theme.buildStatus(startedItem) != theme.buildStatus(reviewItem));
    }

    SECTION("buildStatus — closed item has same length as open item")
    {
        ListItemEntity openItem = buildOpenItem("aaaa", "test", "high", "queued", listName);
        ListItemEntity closedItem = buildClosedItem("aaaa", "test", listName);

        REQUIRE(StringHelpers::countCharsWithoutBashCodes(theme.buildStatus(openItem)) ==
                StringHelpers::countCharsWithoutBashCodes(theme.buildStatus(closedItem)));
    }

    // ---- buildValue (additional) ------------------------------------------------

    SECTION("buildValue — closed item has different styling than open item")
    {
        ListItemEntity openItem = buildOpenItem("aaaa", "same text", "high", "queued", listName);
        ListItemEntity closedItem = buildClosedItem("aaaa", "same text", listName);

        REQUIRE(theme.buildValue(openItem, 0) != theme.buildValue(closedItem, 0));
    }

    SECTION("buildValue — leftOffset reduces wrap threshold")
    {
        std::string longValue;
        for (int i = 0; i < 30; i++)
            longValue += "word" + std::to_string(i) + " ";
        ListItemEntity item = buildOpenItem("aaaa", longValue, "high", "queued", listName);

        std::string noOffset = theme.buildValue(item, 0);
        std::string withOffset = theme.buildValue(item, 12);
        REQUIRE(noOffset != withOffset);
    }

    // ---- autoLineBreak edge cases (via buildValue, private) ---------------------

    SECTION("buildValue — single word longer than column width wraps with newline")
    {
        Default narrowTheme(ioService, listService, listItemService, 80, 80);
        std::string longWord(80, 'x');
        ListItemEntity item = buildOpenItem("aaaa", longWord, "high", "queued", listName);
        std::string result = narrowTheme.buildValue(item, 0);
        REQUIRE(result.find('\n') != std::string::npos);
    }

    SECTION("buildValue — building item with empty value throws at construction")
    {
        REQUIRE_THROWS(buildOpenItem("aaaa", "", "high", "queued", listName));
    }

    SECTION("buildValue — single short word does not wrap")
    {
        Default narrowTheme(ioService, listService, listItemService, 80, 80);
        ListItemEntity item = buildOpenItem("aaaa", "hi", "high", "queued", listName);
        std::string result = narrowTheme.buildValue(item, 0);
        REQUIRE(result.find("hi") != std::string::npos);
    }

    SECTION("buildValue — larger offset produces different wrapping than zero offset")
    {
        Default narrowTheme(ioService, listService, listItemService, 80, 80);
        std::string longValue;
        for (int i = 0; i < 15; i++)
            longValue += "word" + std::to_string(i) + " ";
        ListItemEntity item = buildOpenItem("aaaa", longValue, "high", "queued", listName);
        REQUIRE(narrowTheme.buildValue(item, 0) != narrowTheme.buildValue(item, 20));
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
        std::vector<ListName> listNames = { listName };
        std::vector<ListItemEntity> emptyItems;
        REQUIRE_NOTHROW(theme.printMultipleList(listNames, emptyItems));
    }

    SECTION("print — with items does not throw")
    {
        std::vector<ListItemEntity> items = {
            buildOpenItem("aaaa", "first task", "high", "queued", listName),
            buildOpenItem("bbbb", "second task", "medium", "started", listName),
        };
        REQUIRE_NOTHROW(theme.print(listName, items, false, true));
    }

    SECTION("print — showListName=true does not throw")
    {
        std::vector<ListItemEntity> items = { buildOpenItem("aaaa", "task", "high", "queued", listName) };
        REQUIRE_NOTHROW(theme.print(listName, items, true, true));
    }

    SECTION("print — showTitle=false does not throw")
    {
        std::vector<ListItemEntity> items = { buildOpenItem("aaaa", "task", "high", "queued", listName) };
        REQUIRE_NOTHROW(theme.print(listName, items, false, false));
    }

    SECTION("printMultipleList — with items does not throw")
    {
        std::vector<ListName> listNames = { listName };
        std::vector<ListItemEntity> items = { buildOpenItem("aaaa", "task one", "high", "queued", listName) };
        REQUIRE_NOTHROW(theme.printMultipleList(listNames, items));
    }

    // ---- printMultipleList with multiple lists ----------------------------------

    SECTION("printMultipleList — two lists with items does not throw")
    {
        ListName listName2 = listService.createListName("tempList2Name");
        std::vector<ListItemEntity> items = { buildOpenItem("aaaa", "task one", "high", "queued", listName) };
        std::vector<ListName> listNames = { listName, listName2 };
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

    // ---- buildShortStatsCounts --------------------------------------------------

    SECTION("buildShortStatsCounts — result is not empty")
    {
        ListCountSummary summary;
        REQUIRE_FALSE(theme.buildShortStatsCounts(summary).empty());
    }

    SECTION("buildShortStatsCounts — all-zero summary does not throw")
    {
        ListCountSummary summary;
        REQUIRE_NOTHROW(theme.buildShortStatsCounts(summary));
    }

    SECTION("buildShortStatsCounts — total count appears in result")
    {
        ListCountSummary summary;
        summary.total = 42;
        REQUIRE(theme.buildShortStatsCounts(summary).find("42") != std::string::npos);
    }

    SECTION("buildShortStatsCounts — archived count appears in result")
    {
        ListCountSummary summary;
        summary.archived = 17;
        REQUIRE(theme.buildShortStatsCounts(summary).find("17") != std::string::npos);
    }

    SECTION("buildShortStatsCounts — delivered count appears in result")
    {
        ListCountSummary summary;
        summary.delivered = 99;
        REQUIRE(theme.buildShortStatsCounts(summary).find("99") != std::string::npos);
    }

    SECTION("buildShortStatsCounts — cancelled count appears in result")
    {
        ListCountSummary summary;
        summary.cancelled = 7;
        REQUIRE(theme.buildShortStatsCounts(summary).find("7") != std::string::npos);
    }

    SECTION("buildShortStatsCounts — deleted count appears in result")
    {
        ListCountSummary summary;
        summary.deleted = 3;
        REQUIRE(theme.buildShortStatsCounts(summary).find("3") != std::string::npos);
    }

    SECTION("buildShortStatsCounts — all distinct values appear together")
    {
        ListCountSummary summary;
        summary.total = 100;
        summary.archived = 200;
        summary.delivered = 300;
        summary.cancelled = 400;
        summary.deleted = 500;
        std::string result = theme.buildShortStatsCounts(summary);
        REQUIRE(result.find("100") != std::string::npos);
        REQUIRE(result.find("200") != std::string::npos);
        REQUIRE(result.find("300") != std::string::npos);
        REQUIRE(result.find("400") != std::string::npos);
        REQUIRE(result.find("500") != std::string::npos);
    }

    SECTION("buildShortStatsCounts — two different summaries produce different results")
    {
        ListCountSummary summaryA;
        summaryA.total = 5;
        ListCountSummary summaryB;
        summaryB.total = 10;
        REQUIRE(theme.buildShortStatsCounts(summaryA) != theme.buildShortStatsCounts(summaryB));
    }
}

TEST_CASE("Theme child rows", "[ThemeAbstract]")
{
    IOService ioService("cli");
    ConfSerializer confService(ioService);
    JsonSerializer jsonService(ioService);
    std::unique_ptr<DataSerializerInterface> fileDataStorageServicePtr = std::make_unique<JsonSerializer>(ioService);
    std::unique_ptr<DataSerializerInterface> fileDataConfigStorageServicePtr =
        std::make_unique<ConfSerializer>(ioService);
    MockAppInitialization init(ioService, "_todoos_ThemeChildRowsTest");
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
    DescriptionRepository descriptionRepository(configService.getDescriptionsDirPath());
    ListItemService listItemService(
        ioService, configService, listItemRepository, descriptionRepository, priorityService, statusService);
    std::unique_ptr<DataSerializerInterface> jsonFileDataStorageServicePtr = std::make_unique<JsonSerializer>(ioService);
    ListRepository listRepository(configService, jsonFileDataStorageServicePtr.get());
    EventBus bus;
    ListService listService(ioService, configService, listRepository, bus);
    ListName listName = listService.createUsedListName();

    ChildRowTheme theme(ioService, listService, listItemService, 140, 140);
    theme.fitIdColumn({});

    SECTION("a child row has the same visible width as a top-level row; its text starts after the connector")
    {
        ListItemEntity item = buildOpenItem("aaaa", "short value", "high", "queued", listName);
        std::vector<std::string> topLevel = visibleLines(theme.printListRow(item, true));
        std::vector<std::string> child = visibleLines(theme.childRow(item, "└─ "));

        REQUIRE(topLevel.size() == 1);
        REQUIRE(child.size() == 1);
        REQUIRE(StringHelpers::countCharsWithoutBashCodes(child[0]) ==
                StringHelpers::countCharsWithoutBashCodes(topLevel[0]));
        REQUIRE(columnOf(child[0], "└─ ") == columnOf(topLevel[0], "short value"));
        REQUIRE(columnOf(child[0], "short value") == columnOf(topLevel[0], "short value") + 3);
    }

    SECTION("wrapped child lines line up under the child's text; `├─` keeps a `│`, `└─` does not")
    {
        std::string longValue;
        for (int i = 0; i < 12; i++) {
            longValue += "wrapping words ";
        }
        longValue += "end";
        ListItemEntity item = buildOpenItem("aaaa", longValue, "high", "queued", listName);

        std::vector<std::string> middle = visibleLines(theme.childRow(item, "├─ "));
        REQUIRE(middle.size() >= 2);
        const long connectorColumn = columnOf(middle[0], "├─ ");
        for (size_t i = 1; i < middle.size(); i++) {
            REQUIRE(columnOf(middle[i], "│") == connectorColumn);
            // the text starts 3 columns after the connector column: "│" + 2 spaces
            REQUIRE(textColumn(middle[i]) == connectorColumn + 3);
        }

        std::vector<std::string> last = visibleLines(theme.childRow(item, "└─ "));
        REQUIRE(last.size() == middle.size());
        for (size_t i = 1; i < last.size(); i++) {
            REQUIRE(last[i].find("│") == std::string::npos);
            REQUIRE(textColumn(last[i]) == connectorColumn + 3);
        }

        // the last line ends at the same column as a top-level row's last line
        std::vector<std::string> topLevel = visibleLines(theme.printListRow(item, true));
        REQUIRE(StringHelpers::countCharsWithoutBashCodes(last.back()) ==
                StringHelpers::countCharsWithoutBashCodes(topLevel.back()));
    }

    installation.wipe();
}
