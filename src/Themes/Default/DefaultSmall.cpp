#include "DefaultSmall.h"

DefaultSmall::DefaultSmall(IOService& ioService,
                           ListService& listService,
                           ListItemService& listItemService,
                           int consoleWidth,
                           int consoleRowMaxLength)
  : ThemeAbstract(ioService, listService, listItemService, consoleWidth, consoleRowMaxLength - 10)
{
    statsWhenLength = 18;
    statsCreatedCompletedLength = 10;
    statsPercentageLength = 8;
}

void
DefaultSmall::printListName(std::vector<ListName>& listNames)
{
    std::string titleListName = "";
    for (auto listName : listNames) {
        titleListName += listName.getName() + " ";
    }
    titleListName.pop_back();

    // Left side: total + archived
    int totalCount = 0;
    for (auto listName : listNames) {
        totalCount += listItemService.count(listName);
    }
    int archivedCount = 0;
    for (auto listName : listNames) {
        ListName listNameArchive = ListName::createVariant(listName, "archive");
        archivedCount += listItemService.count(listNameArchive);
    }
    std::string leftSide = "📈 " + std::to_string(totalCount) + "  🚀 " + std::to_string(archivedCount);
    int leftLen = static_cast<int>(StringHelpers::countCharsWithoutBashCodes(leftSide));

    int separator = consoleRowLength - leftLen;
    if (separator < 1)
        separator = 1;
    std::string showCount = leftSide + StringHelpers::adjustStringLength("", separator);

    // List name title
    if (currentListVariant == "archive") {
        titleListName = StringHelpers::colorize(StringHelpers::toUpper(titleListName + " archived"), LIGHT_YELLOW);
    } else if (currentListVariant == "delete") {
        titleListName = StringHelpers::colorize(StringHelpers::toUpper(titleListName + " deleted"), LIGHT_RED);
    } else {
        titleListName = StringHelpers::colorize(StringHelpers::toUpper(titleListName), WHITE);
    }

    int listNameLength = static_cast<int>(StringHelpers::countCharsWithoutBashCodes(titleListName));
    int paddingLength = (consoleRowLength - listNameLength) / 2;
    std::string paddingLeft =
        StringHelpers::colorize(StringHelpers::adjustStringLengthWithString("", paddingLength, "─"), GRAY);
    std::string paddingRight =
        StringHelpers::colorize(StringHelpers::adjustStringLengthWithString("", paddingLength, "─"), GRAY);

    if ((consoleRowLength - listNameLength) % 2 != 0) {
        titleListName += StringHelpers::colorize("─", GRAY);
    }

    std::string line1 = " ╔═══" + StringHelpers::adjustStringLengthWithString("═", consoleRowLength, "═") + "═══╗";
    std::string line2 = " ║   " + paddingLeft + titleListName + paddingRight + "   ║";
    std::string line3 = " ║   " + showCount + "   ║";
    std::string line4 = " ╚═══" + StringHelpers::adjustStringLengthWithString("═", consoleRowLength, "═") + "═══╝";

    ioService.print(StringHelpers::colorize(line1, WHITE));
    ioService.print(line2);
    ioService.print(line3);
    ioService.print(StringHelpers::colorize(line4, WHITE));
    ioService.br();
}

std::string
DefaultSmall::buildTitle()
{
    std::string line = "";
    // ID
    line += StringHelpers::adjustStringLength(" ID", ID_LENGTH);
    // Spacing
    line += StringHelpers::adjustStringLength("", PRIORITY_LENGTH);
    // Value
    line += StringHelpers::adjustStringLength("TITLE", consoleRowLength);
    return line;
}

std::string
DefaultSmall::buildLine(const ListItemEntity& listItemEntity, bool hideListNameInLine)
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
        line += StringHelpers::adjustStringLength((*listItemEntity.getListName()).getName() + " ", LISTNAME_LENGTH);
    }
    line += buildValue(listItemEntity, listNameLeftOffset);
    return line;
}

void
DefaultSmall::printATitle(std::string titleLine1, std::string titleLine2)
{
    int paddingLengthLine1 = (consoleRowLength - static_cast<int>(titleLine1.length())) / 2;
    int paddingLengthLine2 = (consoleRowLength - static_cast<int>(titleLine2.length())) / 2;
    std::string paddingLeftLine1 =
        StringHelpers::colorize(StringHelpers::adjustStringLengthWithString("", paddingLengthLine1, "─"), GRAY);
    std::string paddingRightLine1 =
        StringHelpers::colorize(StringHelpers::adjustStringLengthWithString("", paddingLengthLine1, "─"), GRAY);
    std::string paddingLeftLine2 =
        StringHelpers::colorize(StringHelpers::adjustStringLengthWithString("", paddingLengthLine2, "─"), GRAY);
    std::string paddingRightLine2 =
        StringHelpers::colorize(StringHelpers::adjustStringLengthWithString("", paddingLengthLine2, "─"), GRAY);

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
DefaultSmall::printStats(ListName& listName)
{
    ListName listNameArchive = ListName::createVariant(listName, "archive");
    ListName listNameDelete = ListName::createVariant(listName, "delete");

    std::string totalEmoji = " 📄 Total: ";
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

    std::string pauseEmoji = " 🚧 Paused: ";
    std::string pauseCount = std::to_string(listItemService.countWithStatus(listName, { StatusService::PAUSED }));
    pauseCount = pauseEmoji + pauseCount;

    std::string completedEmoji = " ✅ Completed: ";
    std::string completedCount =
        std::to_string(listItemService.countWithStatus(listName, { StatusService::COMPLETED }));
    completedCount = completedEmoji + completedCount;

    std::string cancelEmoji = " 🪧 Cancelled: ";
    std::string cancelCount = std::to_string(listItemService.countWithStatus(listName, { StatusService::CANCELLED }));
    cancelCount = cancelEmoji + cancelCount;

    std::string archivedEmoji = " 🚀 Archived: ";
    std::string archivedCount = std::to_string(listItemService.count(listNameArchive));
    archivedCount = archivedEmoji + archivedCount;

    std::string cancelledArchivedEmoji = "  🚫 Cancelled: ";
    std::string cancelledArchivedCount =
        std::to_string(listItemService.countWithStatus(listNameArchive, { StatusService::CANCELLED }));
    cancelledArchivedCount = cancelledArchivedEmoji + cancelledArchivedCount;

    std::string deletedEmoji = "  🧹 Deleted: ";
    std::string deletedCount = std::to_string(listItemService.count(listNameDelete));
    deletedCount = deletedEmoji + deletedCount;

    std::string criticalEmoji = StringHelpers::colorize(" ■ ", WHITE) + "Critical: ";
    std::string criticalCount =
        std::to_string(listItemService.countWithPriority(listName, { PriorityService::CRITICAL })) + " ";
    criticalCount = criticalEmoji + criticalCount;

    std::string urgentEmoji = StringHelpers::colorize(" ▲ ", RED) + "Urgent: ";
    std::string urgentCount =
        std::to_string(listItemService.countWithPriority(listName, { PriorityService::URGENT })) + " ";
    urgentCount = urgentEmoji + urgentCount;

    std::string highEmoji = StringHelpers::colorize(" ▶ ", ORANGE) + "High: ";
    std::string highCount =
        std::to_string(listItemService.countWithPriority(listName, { PriorityService::HIGH })) + " ";
    highCount = highEmoji + highCount;

    std::string mediumEmoji = StringHelpers::colorize(" ▼ ", LIGHT_GREEN) + "Medium: ";
    std::string mediumCount =
        std::to_string(listItemService.countWithPriority(listName, { PriorityService::MEDIUM })) + " ";
    mediumCount = mediumEmoji + mediumCount;

    std::string lowEmoji = StringHelpers::colorize(" ▽ ", GREEN) + "Low: ";
    std::string lowCount = std::to_string(listItemService.countWithPriority(listName, { PriorityService::LOW }));
    lowCount = lowEmoji + lowCount;

    ioService.printWithoutLineBreak(total);
    ioService.printWithoutLineBreak(todoCount);
    ioService.printWithoutLineBreak(startedCount);
    ioService.printWithoutLineBreak(underReviewCount);
    ioService.printWithoutLineBreak(pauseCount);
    ioService.printWithoutLineBreak(completedCount);
    ioService.printWithoutLineBreak(cancelCount);
    ioService.br();
    ioService.br();
    ioService.printWithoutLineBreak(archivedCount);
    ioService.printWithoutLineBreak(cancelledArchivedCount);
    ioService.printWithoutLineBreak(deletedCount);
    ioService.br();
    ioService.br();
    ioService.printWithoutLineBreak(criticalCount);
    ioService.printWithoutLineBreak(urgentCount);
    ioService.printWithoutLineBreak(highCount);
    ioService.printWithoutLineBreak(mediumCount);
    ioService.printWithoutLineBreak(lowCount);
    ioService.br();
    ioService.br();
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
