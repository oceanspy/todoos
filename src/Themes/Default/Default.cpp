#include "Default.h"
#include <string>

Default::Default(IOService& ioService,
                 ListService& listService,
                 ListItemService& listItemService,
                 int consoleWidth,
                 int consoleRowMaxLength)
  : ThemeAbstract(ioService, listService, listItemService, consoleWidth, consoleRowMaxLength - 56)
{
    statsWhenLength = 26;
    statsCreatedCompletedLength = 20;
    statsPercentageLength = 8;
}

void
Default::printListTitle(ListName& listName)
{
    ListCountSummary summary = listItemService.getCountSummary({ listName });

    std::string totalStr = std::to_string(summary.total);
    int totalCharLength = 3 + static_cast<int>(totalStr.length());
    totalStr = "📈 " + totalStr;

    std::string archivedStr = std::to_string(summary.archived);
    int archivedCharLength = 6 + static_cast<int>(archivedStr.length());
    archivedStr = "   ⚡ " + archivedStr;

    std::string deliveredStr = std::to_string(summary.delivered);
    int deliveredCharLength = 6 + static_cast<int>(deliveredStr.length());
    deliveredStr = " / 🚀 " + deliveredStr;

    std::string cancelledStr = std::to_string(summary.cancelled);
    int cancelledCharLength = 4 + static_cast<int>(cancelledStr.length());
    cancelledStr = " ✖️ " + cancelledStr;

    std::string deletedStr = std::to_string(summary.deleted);
    int deletedCharLength = 4 + static_cast<int>(deletedStr.length());
    deletedStr = " 🧹 " + deletedStr;

    std::string statusPrintCount = totalStr + archivedStr + deliveredStr + cancelledStr + deletedStr;
    int statusCountLength =
        totalCharLength + archivedCharLength + deliveredCharLength + cancelledCharLength + deletedCharLength;

    std::string line1 = " > " + buildListTitle(listName);
    std::string line2 = "   " + buildListLastUpdate(summary.getLastUpdate());
    std::string line3 = "   " + StringHelpers::colorize("──────────────────────────────", GRAY);
    std::string line4 = "   " + statusPrintCount;

    ioService.brOrSkip();
    ioService.print(line1);
    ioService.print(line2);
    ioService.print(line3);
    ioService.print(line4);
    ioService.br();
}

std::string
Default::buildListTitle(ListName& listName)
{
    std::string title;
    title = StringHelpers::toUpper(listName.getName());
    title = StringHelpers::colorize(title, BOLD);

    if (listName.getVariant() == "delete") {
        title += StringHelpers::colorize(" (deleted items)", YELLOW);
    } else if (listName.getVariant() == "archive") {
        title += StringHelpers::colorize(" (archived items)", CYAN);
    }

    return title;
}

std::string
Default::buildListLastUpdate(const time_t& time)
{
    std::string updatedAt = StringHelpers::colorize(
        StringHelpers::colorize("Updated at:" + DateHelpers::formatTimestampToHumanDate(time), GRAY), ITALIC);
    return updatedAt;
}

std::string
Default::buildPriorityCounts(const ListCountSummary& summary)
{
    return StringHelpers::colorize("■ ", WHITE) + std::to_string(summary.getPriority(PriorityService::CRITICAL)) + " " +
           StringHelpers::colorize("● ", RED) + std::to_string(summary.getPriority(PriorityService::URGENT)) + " " +
           StringHelpers::colorize("● ", ORANGE) + std::to_string(summary.getPriority(PriorityService::HIGH)) + " " +
           StringHelpers::colorize("● ", LIGHT_GREEN) + std::to_string(summary.getPriority(PriorityService::MEDIUM)) +
           " " + StringHelpers::colorize("◌ ", GREEN) + std::to_string(summary.getPriority(PriorityService::LOW));
}

std::string
Default::printListTitleRow()
{
    std::string line = "";
    // ID
    line += StringHelpers::adjustStringLength(" ID", ID_LENGTH);
    // Spacing
    line += StringHelpers::adjustStringLength("", PRIORITY_LENGTH);
    // Value
    line += StringHelpers::adjustStringLength("TITLE", consoleRowLength);
    // Status
    line += StringHelpers::adjustStringLength("STATUS", STATUS_LENGTH);
    // Date
    line += StringHelpers::adjustStringLength("DATE", DATE_LENGTH);
    return line;
}

std::string
Default::printListRow(const ListItemEntity& listItemEntity, bool hideListNameInLine)
{
    int listNameLeftOffset = 0;
    std::string line = "";

    // ID
    line += buildId(listItemEntity);
    // Priority
    line += buildPriority(listItemEntity);
    // Value
    if (!hideListNameInLine) {
        listNameLeftOffset = 12;
        std::string listName = line +=
            StringHelpers::adjustStringLength((*listItemEntity.getListName()).getName() + " ", LISTNAME_LENGTH);
    }
    line += buildValue(listItemEntity, listNameLeftOffset);
    // Status
    line += buildStatus(listItemEntity);
    // Date
    line += buildDate(listItemEntity);
    return line;
}

void
Default::printATitle(std::string titleLine1, std::string titleLine2)
{
    int titleLength = titleLine1.length() > titleLine2.length() ? static_cast<int>(titleLine1.length())
                                                                : static_cast<int>(titleLine2.length());

    if (consoleRowLength > titleLength * 2) {
        consoleRowLength = titleLength * 2 > 60 ? titleLength * 2 : 60;
    }
    int paddingLengthLine1 = (consoleRowLength - static_cast<int>(titleLine1.length())) / 2;
    int paddingLengthLine2 = (consoleRowLength - static_cast<int>(titleLine2.length())) / 2;
    std::string paddingLeftLine1 =
        StringHelpers::colorize(StringHelpers::adjustStringLengthWithString("", paddingLengthLine1, "─"), GRAY);
    std::string paddingRightLine1 =
        StringHelpers::colorize(StringHelpers::adjustStringLengthWithString("", paddingLengthLine1, "─"), GRAY);
    std::string paddingLeftLine2 =
        StringHelpers::colorize(StringHelpers::adjustStringLengthWithString("", paddingLengthLine2, " "), GRAY);
    std::string paddingRightLine2 =
        StringHelpers::colorize(StringHelpers::adjustStringLengthWithString("", paddingLengthLine2, " "), GRAY);

    if (static_cast<int>(titleLine1.length()) % 2 != 0) {
        paddingRightLine1 += " ";
    }
    if (static_cast<int>(titleLine2.length()) % 2 != 0) {
        paddingRightLine2 += " ";
    }

    std::string line1 = " ╔═══" + StringHelpers::adjustStringLengthWithString("═", consoleRowLength, "═") + "═══╗";
    std::string line2 = " ║   " + paddingLeftLine1 + StringHelpers::toUpper(titleLine1) + paddingRightLine1 + "   ║";
    std::string line3 = " ║   " + paddingLeftLine2 + StringHelpers::toUpper(titleLine2) + paddingRightLine2 + "   ║";
    std::string line4 = " ╚═══" + StringHelpers::adjustStringLengthWithString("═", consoleRowLength, "═") + "═══╝";

    ioService.print(StringHelpers::colorize(line1, WHITE));
    ioService.print(line2);
    if (!titleLine2.empty()) {
        ioService.print(line3);
    }
    ioService.print(StringHelpers::colorize(line4, WHITE));
    ioService.br();
}

void
Default::printStats(ListName& listName)
{
    ListName listNameArchive = ListName::createVariant(listName, "archive");
    ListName listNameDelete = ListName::createVariant(listName, "delete");

    std::string totalEmoji = " 📈 Total: ";
    std::string total = std::to_string(listItemService.count(listName));
    total = totalEmoji + total;

    std::string todoEmoji = " ⏳ To-Do: ";
    std::string todoCount = std::to_string(listItemService.countWithStatus(listName, { StatusService::TO_DO }));
    todoCount = todoEmoji + todoCount;

    std::string startedEmoji = " 🏃 Started: ";
    std::string startedCount = std::to_string(listItemService.countWithStatus(listName, { StatusService::STARTED }));
    startedCount = startedEmoji + startedCount;

    std::string underReviewEmoji = " 🔍 Reviewing: ";
    std::string underReviewCount =
        std::to_string(listItemService.countWithStatus(listName, { StatusService::REVIEWING }));
    underReviewCount = underReviewEmoji + underReviewCount;

    std::string pauseEmoji = " 💤 Paused: ";
    std::string pauseCount = std::to_string(listItemService.countWithStatus(listName, { StatusService::PAUSED }));
    pauseCount = pauseEmoji + pauseCount;

    std::string archivedEmoji = " 🚀 Delivered: ";
    int archivedCount = listItemService.countWithStatus(listName, { StatusService::COMPLETED });
    archivedCount += listItemService.countWithStatus(listNameArchive, { StatusService::COMPLETED });
    std::string archivedCountStr = archivedEmoji + std::to_string(archivedCount);

    std::string cancelEmoji = " ✖️ Cancelled: ";
    int cancelCount = listItemService.countWithStatus(listName, { StatusService::CANCELLED });
    cancelCount += listItemService.countWithStatus(listNameArchive, { StatusService::CANCELLED });
    std::string cancelCountStr = cancelEmoji + std::to_string(cancelCount);

    std::string deletedEmoji = "  🧹 Deleted: ";
    std::string deletedCount = std::to_string(listItemService.count(listNameDelete));
    deletedCount = deletedEmoji + deletedCount;

    std::string criticalEmoji = StringHelpers::colorize(" ■ ", WHITE) + "Critical: ";
    std::string criticalCount =
        std::to_string(listItemService.countWithPriority(listName, { PriorityService::CRITICAL })) + " ";
    criticalCount = criticalEmoji + criticalCount;

    std::string urgentEmoji = StringHelpers::colorize(" ● ", RED) + "Urgent: ";
    std::string urgentCount =
        std::to_string(listItemService.countWithPriority(listName, { PriorityService::URGENT })) + " ";
    urgentCount = urgentEmoji + urgentCount;

    std::string highEmoji = StringHelpers::colorize(" ● ", ORANGE) + "High: ";
    std::string highCount =
        std::to_string(listItemService.countWithPriority(listName, { PriorityService::HIGH })) + " ";
    highCount = highEmoji + highCount;

    std::string mediumEmoji = StringHelpers::colorize(" ● ", LIGHT_GREEN) + "Medium: ";
    std::string mediumCount =
        std::to_string(listItemService.countWithPriority(listName, { PriorityService::MEDIUM })) + " ";
    mediumCount = mediumEmoji + mediumCount;

    std::string lowEmoji = StringHelpers::colorize(" ◌ ", GREEN) + "Low: ";
    std::string lowCount = std::to_string(listItemService.countWithPriority(listName, { PriorityService::LOW }));
    lowCount = lowEmoji + lowCount;

    ioService.printWithoutLineBreak(total);
    ioService.printWithoutLineBreak(todoCount);
    ioService.printWithoutLineBreak(startedCount);
    ioService.printWithoutLineBreak(underReviewCount);
    ioService.printWithoutLineBreak(pauseCount);
    ioService.br();
    ioService.printWithoutLineBreak(archivedCountStr);
    ioService.printWithoutLineBreak(cancelCountStr);
    ioService.printWithoutLineBreak(deletedCount);
    ioService.br();
    ioService.printWithoutLineBreak(criticalCount);
    ioService.printWithoutLineBreak(urgentCount);
    ioService.printWithoutLineBreak(highCount);
    ioService.printWithoutLineBreak(mediumCount);
    ioService.printWithoutLineBreak(lowCount);
    ioService.br();
    ioService.br();
    ioService.printWithoutLineBreak(StringHelpers::adjustStringLength(" WHEN", statsWhenLength));
    ioService.printWithoutLineBreak(StringHelpers::adjustStringLength("COMPL./CREAT.", statsCreatedCompletedLength));
    ioService.printWithoutLineBreak(StringHelpers::adjustStringLength("PERC.", statsPercentageLength));
    ioService.br();
    printFullLine(GRAY);
    renderListStatLine(listName, DateHelpers::getTodayStart(), DateHelpers::getTodayEnd(), "today");
    renderListStatLine(listName, DateHelpers::getTodayStart(-1), DateHelpers::getTodayEnd(-1), "yesterday");
    ioService.br();
    renderListStatLine(listName,
                       DateHelpers::getDayStart("monday", time(nullptr)),
                       DateHelpers::getDayEnd("monday", time(nullptr)),
                       "monday");
    renderListStatLine(listName,
                       DateHelpers::getDayStart("tuesday", time(nullptr)),
                       DateHelpers::getDayEnd("tuesday", time(nullptr)),
                       "tuesday");
    renderListStatLine(listName,
                       DateHelpers::getDayStart("wednesday", time(nullptr)),
                       DateHelpers::getDayEnd("wednesday", time(nullptr)),
                       "wednesday");
    renderListStatLine(listName,
                       DateHelpers::getDayStart("thursday", time(nullptr)),
                       DateHelpers::getDayEnd("thursday", time(nullptr)),
                       "thursday");
    renderListStatLine(listName,
                       DateHelpers::getDayStart("friday", time(nullptr)),
                       DateHelpers::getDayEnd("friday", time(nullptr)),
                       "friday");
    renderListStatLine(listName,
                       DateHelpers::getDayStart("saturday", time(nullptr)),
                       DateHelpers::getDayEnd("saturday", time(nullptr)),
                       "saturday");
    renderListStatLine(listName,
                       DateHelpers::getDayStart("sunday", time(nullptr)),
                       DateHelpers::getDayEnd("sunday", time(nullptr)),
                       "sunday");
    ioService.br();
    renderListStatLine(listName, DateHelpers::getWeekStart(), DateHelpers::getWeekEnd(), "this week");
    renderListStatLine(listName, DateHelpers::getWeekStart(-1), DateHelpers::getWeekEnd(-1), "last week");
    ioService.br();
    renderListStatLine(listName, DateHelpers::getMonthStart(), DateHelpers::getMonthEnd(), "this month");
    renderListStatLine(listName, DateHelpers::getMonthStart(-1), DateHelpers::getMonthEnd(-1), "last month");
    ioService.br();
    renderListStatLine(listName, DateHelpers::getYearStart(), DateHelpers::getYearEnd(), "this year");
    renderListStatLine(listName, 0, time(nullptr), "since started");
}
