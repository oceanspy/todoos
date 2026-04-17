#include "DefaultDetailed.h"
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

    std::string titleListName = listNameRendered(listName);

    std::string totalStr = std::to_string(summary.total);
    int totalCharLength = 3 + static_cast<int>(totalStr.length());
    totalStr = "📈 " + totalStr;

    std::string todoStr = std::to_string(summary.getStatus(StatusService::QUEUED));
    int todoCharLength = 6 + static_cast<int>(todoStr.length());
    todoStr = "   ⏳ " + todoStr;

    std::string startedStr = std::to_string(summary.getStatus(StatusService::STARTED));
    int startedCharLength = 4 + static_cast<int>(startedStr.length());
    startedStr = " 🏃 " + startedStr;

    std::string underReviewStr = std::to_string(summary.getStatus(StatusService::TRIAGED));
    int underReviewCharLength = 4 + static_cast<int>(underReviewStr.length());
    underReviewStr = " 🔍 " + underReviewStr;

    std::string pauseStr = std::to_string(summary.getStatus(StatusService::PAUSED));
    int pauseCharLength = 4 + static_cast<int>(pauseStr.length());
    pauseStr = " 💤 " + pauseStr;

    std::string blockedStr = std::to_string(summary.getStatus(StatusService::BLOCKED));
    int blockedCharLength = 4 + static_cast<int>(blockedStr.length());
    blockedStr = " 🚫 " + blockedStr;

    std::string completedStr = std::to_string(summary.getStatus(StatusService::COMPLETED));
    int completedCharLength = 4 + static_cast<int>(completedStr.length());
    completedStr = " ✅ " + completedStr;

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

    std::string statusPrintCount = totalStr + todoStr + startedStr + underReviewStr + pauseStr + blockedStr +
                                   completedStr + archivedStr + deliveredStr + cancelledStr + deletedStr;
    int statusCountLength = totalCharLength + todoCharLength + startedCharLength + pauseCharLength + blockedCharLength +
                            completedCharLength + underReviewCharLength + archivedCharLength + deliveredCharLength +
                            cancelledCharLength + deletedCharLength;

    std::string criticalStr =
        StringHelpers::colorize("■ ", WHITE) + std::to_string(summary.getPriority(PriorityService::CRITICAL)) + " ";
    int criticalCharLength =
        2 + static_cast<int>(std::to_string(summary.getPriority(PriorityService::CRITICAL)).length()) + 1;

    std::string urgentStr =
        StringHelpers::colorize("● ", RED) + std::to_string(summary.getPriority(PriorityService::URGENT)) + " ";
    int urgentCharLength =
        2 + static_cast<int>(std::to_string(summary.getPriority(PriorityService::URGENT)).length()) + 1;

    std::string highStr =
        StringHelpers::colorize("● ", ORANGE) + std::to_string(summary.getPriority(PriorityService::HIGH)) + " ";
    int highCharLength = 2 + static_cast<int>(std::to_string(summary.getPriority(PriorityService::HIGH)).length()) + 1;

    std::string mediumStr =
        StringHelpers::colorize("● ", LIGHT_GREEN) + std::to_string(summary.getPriority(PriorityService::MEDIUM)) + " ";
    int mediumCharLength =
        2 + static_cast<int>(std::to_string(summary.getPriority(PriorityService::MEDIUM)).length()) + 1;

    std::string lowStr =
        StringHelpers::colorize("◌ ", GREEN) + std::to_string(summary.getPriority(PriorityService::LOW)) + " ";
    int lowCharLength = 2 + static_cast<int>(std::to_string(summary.getPriority(PriorityService::LOW)).length()) + 1;

    std::string priorityPrintCount = criticalStr + urgentStr + highStr + mediumStr + lowStr;
    int priorityCountLength = criticalCharLength + urgentCharLength + highCharLength + mediumCharLength + lowCharLength;

    int listTitleLength = consoleRowLength;
    int separator = listTitleLength - (statusCountLength + priorityCountLength);
    if (separator <= 10) {
        listTitleLength += STATUS_LENGTH;
        separator = listTitleLength - (statusCountLength + priorityCountLength);
    }
    if (separator <= 10) {
        statusPrintCount = totalStr + todoStr + startedStr + underReviewStr + pauseStr + deliveredStr + deletedStr;
        statusCountLength = totalCharLength + todoCharLength + startedCharLength + pauseCharLength +
                            underReviewCharLength + deliveredCharLength + deletedCharLength;
        separator = listTitleLength - (statusCountLength + priorityCountLength);
    }
    if (separator <= 10) {
        statusPrintCount = totalStr + todoStr + startedStr + underReviewStr + pauseStr;
        statusCountLength =
            totalCharLength + todoCharLength + startedCharLength + pauseCharLength + underReviewCharLength;
        separator = listTitleLength - (statusCountLength + priorityCountLength);
    }
    if (separator <= 10) {
        statusPrintCount = totalStr + deliveredStr;
        statusCountLength = totalCharLength + deliveredCharLength;
        separator = listTitleLength - (statusCountLength + priorityCountLength);
    }
    if (separator <= 10) {
        statusPrintCount = totalStr;
        statusCountLength = totalCharLength;
        separator = listTitleLength - (statusCountLength + priorityCountLength);
    }
    if (separator <= 10) {
        priorityPrintCount = criticalStr + urgentStr;
        priorityCountLength = criticalCharLength + urgentCharLength;
        separator = listTitleLength - (statusCountLength + priorityCountLength);
    }
    if (separator <= 10) {
        priorityPrintCount = "";
        priorityCountLength = 0;
        separator = listTitleLength - (statusCountLength + priorityCountLength);
    }

    std::string showCount = statusPrintCount + StringHelpers::adjustStringLength("", separator) + priorityPrintCount;

    int listNameLength = static_cast<int>(StringHelpers::countCharsWithoutBashCodes(titleListName));
    int paddingLength = (listTitleLength - listNameLength) / 2;
    std::string paddingLeft =
        StringHelpers::colorize(StringHelpers::adjustStringLengthWithString("", paddingLength, "─"), GRAY);
    std::string paddingRight =
        StringHelpers::colorize(StringHelpers::adjustStringLengthWithString("", paddingLength, "─"), GRAY);

    if ((listTitleLength - listNameLength) % 2 != 0) {
        titleListName += StringHelpers::colorize("─", GRAY);
    }

    std::string line1 = " ╔═══" + StringHelpers::adjustStringLengthWithString("═", listTitleLength, "═") + "═══╗";
    std::string line2 = " ║   " + paddingLeft + titleListName + paddingRight + "   ║";
    std::string line3 = " ║   " + showCount + "   ║";
    std::string line4 = " ╚═══" + StringHelpers::adjustStringLengthWithString("═", listTitleLength, "═") + "═══╝";

    ioService.print(StringHelpers::colorize(line1, WHITE));
    ioService.print(line2);
    ioService.print(line3);
    ioService.print(StringHelpers::colorize(line4, WHITE));
    ioService.br();
}

std::string
Default::buildTitle()
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
Default::buildLine(const ListItemEntity& listItemEntity, bool hideListNameInLine)
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

    std::string todoEmoji = " ⏳ Queued: ";
    std::string todoCount = std::to_string(listItemService.countWithStatus(listName, { StatusService::QUEUED }));
    todoCount = todoEmoji + todoCount;

    std::string startedEmoji = " 🏃 Started: ";
    std::string startedCount = std::to_string(listItemService.countWithStatus(listName, { StatusService::STARTED }));
    startedCount = startedEmoji + startedCount;

    std::string underReviewEmoji = " 🔍 Triaged: ";
    std::string underReviewCount =
        std::to_string(listItemService.countWithStatus(listName, { StatusService::TRIAGED }));
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
