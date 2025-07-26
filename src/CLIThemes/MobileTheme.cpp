#include "MobileTheme.h"
#include <cmath>

MobileTheme::MobileTheme(IOService& ioService,
                         ListService& listService,
                         ListItemService& listItemService,
                         int consoleWidth,
                         int consoleRowMaxLength)
  : ThemeAbstract(ioService, listService, listItemService, consoleWidth)
  , consoleRowLength(consoleRowMaxLength - 10)
{
}

void
MobileTheme::print(ListName& listName, std::vector<ListItemEntity> listItems, bool showListName, bool showTitle)
{
    bool hideListNameInLine = false;
    if (showListName) {
        std::vector<ListName> listNames = { listName };
        printListName(listNames);
        hideListNameInLine = true;
    }

    if (showTitle) {
        std::string title = buildTitle();
        ioService.print(title);
        printFullLine(GRAY);
    }

    if (listItems.empty() && showTitle) {
        ioService.print(" No items found.");
        return;
    }

    for (const ListItemEntity& listItemEntity : listItems) {
        std::string line = buildLine(listItemEntity, hideListNameInLine);
        ioService.print(line);
    }
}

void
MobileTheme::printMultipleList(std::vector<ListName>& listNames, std::vector<ListItemEntity>& listItems)
{
    printListName(listNames);

    std::string title = buildTitle();
    ioService.print(title);
    printFullLine(GRAY);

    if (listItems.empty()) {
        ioService.print(" No items found.");
        return;
    }

    for (const ListItemEntity& listItemEntity : listItems) {
        std::string line = buildLine(listItemEntity, false);
        ioService.print(line);
    }
}

std::string
MobileTheme::buildTitle()
{
    std::string line = "";
    // ID
    line += StringHelpers::adjustStringLength(" ID", ID_LENGTH);
    // Spacing
    line += StringHelpers::adjustStringLength("", PRIORITY_LENGTH);
    // Value
    line += StringHelpers::adjustStringLength("TITLE" + std::to_string(consoleRowLength), consoleRowLength);
    // Status
    //    line += StringHelper::adjustStringLength("STATUS", STATUS_LENGTH);
    // Date
    //    line += StringHelper::adjustStringLength("DATE", DATE_LENGTH);
    return line;
}

std::string
MobileTheme::buildLine(const ListItemEntity& listItemEntity, bool hideListNameInLine)
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
    // Status
    //    line += buildStatus(listItemEntity);
    // Date
    //    line += buildDate(listItemEntity);
    return line;
}

std::string
MobileTheme::buildId(const ListItemEntity& listItemEntity)
{
    if (*(*listItemEntity.status()).isClosed()) {
        return StringHelpers::colorize(" " + *listItemEntity.getId() + " ", GRAY);
    }
    return " " + *listItemEntity.getId() + " ";
}

std::string
MobileTheme::buildStatus(const ListItemEntity& listItemEntity)
{
    std::string render;
    std::string icon = *(*listItemEntity.status()).getIcon() + " ";
    render = !(*listItemEntity.status()).getStyle().empty()
                 ? StringHelpers::colorize(*(*listItemEntity.status()).getName(), (*listItemEntity.status()).getStyle())
                 : *(*listItemEntity.status()).getName();

    int length = STATUS_LENGTH + 1 - (*listItemEntity.status()).getIconLength();
    render = StringHelpers::adjustStringLength(icon + render, length);
    std::string final_render = StringHelpers::colorize(render, (*listItemEntity.status()).getColor());
    return final_render;
}

std::string
MobileTheme::buildPriority(const ListItemEntity& listItemEntity)
{
    if (*(*listItemEntity.status()).isClosed()) {
        return StringHelpers::colorize(" " + *(*listItemEntity.priority()).getIcon() + "  ", GRAY);
    } else {
        return StringHelpers::colorize(" " + *(*listItemEntity.priority()).getIcon() + "  ",
                                       (*listItemEntity.priority()).getColor());
    }
}

std::string
MobileTheme::buildValue(const ListItemEntity& listItemEntity, const int leftOffset)
{
    std::string value = *listItemEntity.getValue();
    value = autoLineBreak(value, consoleRowLength - leftOffset, ID_LENGTH + PRIORITY_LENGTH + leftOffset);
    value = StringHelpers::colorize(value, (*listItemEntity.status()).getColor());
    value = StringHelpers::colorize(value, (*listItemEntity.status()).getStyle());
    return value;
}

std::string
MobileTheme::buildDate(const ListItemEntity& listItemEntity)
{
    if (!*(*listItemEntity.status()).isClosed() && *listItemEntity.getDueAt() > 0) {
        if (*(*listItemEntity.status()).isPassive()) {
            return StringHelpers::colorize(
                StringHelpers::adjustStringLength(
                    "Deadl.: " + DateHelpers::formatTimestampToHumanDate(*listItemEntity.getDueAt(), "date"),
                    DATE_LENGTH),
                (*listItemEntity.status()).getColor());
        }

        // if due date > now + 10 days
        if (*listItemEntity.getDueAt() > time(nullptr) + 864000) {
            return StringHelpers::colorize(
                StringHelpers::adjustStringLength(
                    "Deadl.: " + DateHelpers::formatTimestampToHumanDate(*listItemEntity.getDueAt(), "date"),
                    DATE_LENGTH),
                GREEN);
        }
        // if due date > now + 4 days
        else if (*listItemEntity.getDueAt() > time(nullptr) + 345600) {
            return StringHelpers::colorize(
                StringHelpers::adjustStringLength(
                    "Deadl.: " + DateHelpers::formatTimestampToHumanDate(*listItemEntity.getDueAt(), "date"),
                    DATE_LENGTH),
                LIGHT_YELLOW);
        }
        // if date < now
        else if (*listItemEntity.getDueAt() < time(nullptr) + 86400) {
            return StringHelpers::colorize(
                "Deadl.: " + DateHelpers::formatTimestampToHumanDate(*listItemEntity.getDueAt(), "date"), BG_RED);
        }

        return StringHelpers::colorize(
            StringHelpers::adjustStringLength(
                "Deadl.: " + DateHelpers::formatTimestampToHumanDate(*listItemEntity.getDueAt(), "date"), DATE_LENGTH),
            LIGHT_RED);
    }

    if (!*(*listItemEntity.status()).isClosed()) {
        if (DateHelpers::isTimestampToday(*listItemEntity.getCreatedAt())) {
            return StringHelpers::colorize(
                StringHelpers::adjustStringLength(
                    "Today at " + DateHelpers::formatTimestamp(*listItemEntity.getCreatedAt(), "shortTime"),
                    DATE_LENGTH),
                LIGHT_GREEN);
        } else if (DateHelpers::isTimestampNDaysFromToday(*listItemEntity.getCreatedAt(), -1)) {
            return StringHelpers::colorize(
                StringHelpers::adjustStringLength(
                    "Yesterday at " + DateHelpers::formatTimestamp(*listItemEntity.getCreatedAt(), "shortTime"),
                    DATE_LENGTH),
                GREEN);
        }
        return StringHelpers::adjustStringLength(
            DateHelpers::formatTimestampToHumanDate(*listItemEntity.getCreatedAt()), DATE_LENGTH);
    } else {
        return StringHelpers::colorize(
            StringHelpers::adjustStringLength(
                "Elaps.: " +
                    DateHelpers::timestampToDuration(*listItemEntity.getCreatedAt(), *listItemEntity.getUpdatedAt()),
                DATE_LENGTH),
            GRAY);
    }
}

void
MobileTheme::printListName(std::vector<ListName>& listNames)
{
    std::string titleListName = "";
    for (auto listName : listNames) {
        titleListName += listName.getName() + " ";
    }
    titleListName.pop_back();

    std::string totalEmoji = "📄 ";
    int totalCount = 0;
    for (auto listName : listNames) {
        totalCount += listItemService.count(listName);
    }
    std::string totalStr = std::to_string(totalCount);
    int totalCharLength = 3 + static_cast<int>(totalStr.length());
    totalStr = totalEmoji + totalStr;

    std::string todoEmoji = " ↔ ⏳ ";
    int todoCount = 0;
    for (auto listName : listNames) {
        todoCount += listItemService.countWithStatus(listName, { StatusService::TO_DO });
    }
    std::string todoStr = std::to_string(todoCount);
    int todoCharLength = 6 + static_cast<int>(todoStr.length());
    todoStr = todoEmoji + todoStr;

    std::string startedEmoji = " 🏃 ";
    int startedCount = 0;
    for (auto listName : listNames) {
        startedCount += listItemService.countWithStatus(listName, { StatusService::STARTED });
    }
    std::string startedStr = std::to_string(startedCount);
    int startedCharLength = 4 + static_cast<int>(startedStr.length());
    startedStr = startedEmoji + startedStr;

    std::string underReviewEmoji = " 🔍 ";

    int underReviewCount = 0;
    for (auto listName : listNames) {
        underReviewCount += listItemService.countWithStatus(listName, { StatusService::REVIEWING });
    }
    std::string underReviewStr = std::to_string(underReviewCount);
    int underReviewCharLength = 4 + static_cast<int>(underReviewStr.length());
    underReviewStr = underReviewEmoji + underReviewStr;

    std::string pauseEmoji = " 🚧 ";
    int pauseCount = 0;
    for (auto listName : listNames) {
        pauseCount += listItemService.countWithStatus(listName, { StatusService::PAUSED });
    }
    std::string pauseStr = std::to_string(pauseCount);
    int pauseCharLength = 4 + static_cast<int>(pauseStr.length());
    pauseStr = pauseEmoji + pauseStr;

    std::string pendingEmoji = " 🕛 ";
    int pendingCount = 0;
    for (auto listName : listNames) {
        pendingCount += listItemService.countWithStatus(listName, { StatusService::PENDING });
    }
    std::string pendingStr = std::to_string(pendingCount);
    int pendingCharLength = 4 + static_cast<int>(pendingStr.length());
    pendingStr = pendingEmoji + pendingStr;

    std::string completedEmoji = " ✅ ";
    int completedCount = 0;
    for (auto listName : listNames) {
        completedCount += listItemService.countWithStatus(listName, { StatusService::COMPLETED });
    }
    std::string completedStr = std::to_string(completedCount);
    int completedCharLength = 4 + static_cast<int>(completedStr.length());
    completedStr = completedEmoji + completedStr;

    std::string cancelEmoji = " 🪧 ";
    int cancelCount = 0;
    for (auto listName : listNames) {
        cancelCount += listItemService.countWithStatus(listName, { StatusService::CANCELLED });
    }
    std::string cancelStr = std::to_string(cancelCount);
    int cancelCharLength = 4 + static_cast<int>(cancelStr.length());
    cancelStr = cancelEmoji + cancelStr;

    std::string archivedEmoji = " ↔ 🚀 ";
    int archivedCount = 0;
    for (auto listName : listNames) {
        ListName listNameArchive = ListName::createVariant(listName, "archive");
        archivedCount += listItemService.countWithStatus(listNameArchive, { StatusService::COMPLETED });
    }
    std::string archivedStr = std::to_string(archivedCount);
    int archivedCharLength = 6 + static_cast<int>(archivedStr.length());
    archivedStr = archivedEmoji + archivedStr;

    std::string cancelledArchivedEmoji = " 🚫 ";
    int cancelledCount = 0;
    for (auto listName : listNames) {
        ListName listNameArchive = ListName::createVariant(listName, "archive");
        cancelledCount += listItemService.countWithStatus(listNameArchive, { StatusService::CANCELLED });
    }
    std::string cancelledStr = std::to_string(cancelledCount);
    int cancelledArchivedCharLength = 4 + static_cast<int>(cancelledStr.length());
    cancelledStr = cancelledArchivedEmoji + cancelledStr;

    std::string deletedEmoji = " ↔ 🧹 ";
    int deleteCount = 0;
    for (auto listName : listNames) {
        ListName listNameDelete = ListName::createVariant(listName, "delete");
        deleteCount += listItemService.count(listNameDelete);
    }
    std::string deleteStr = std::to_string(deleteCount);
    int deletedCharLength = 6 + static_cast<int>(deleteStr.length());
    deleteStr = deletedEmoji + deleteStr;

    std::string statusPrintCount = totalStr + todoStr + startedStr + underReviewStr + pauseStr + pendingStr +
                                   completedStr + cancelStr + archivedStr + cancelledStr + deleteStr;
    int statusCountLength = totalCharLength + todoCharLength + startedCharLength + pauseCharLength + pendingCharLength +
                            underReviewCharLength + completedCharLength + cancelCharLength + archivedCharLength +
                            cancelledArchivedCharLength + deletedCharLength;

    std::string criticalEmoji = StringHelpers::colorize("■ ", WHITE);
    int criticalCount = 0;
    for (auto listName : listNames) {
        criticalCount += listItemService.countWithPriority(listName, { PriorityService::CRITICAL });
    }
    std::string criticalStr = std::to_string(criticalCount) + " ";
    int criticalCharLength = 2 + static_cast<int>(criticalStr.length());
    criticalStr = criticalEmoji + criticalStr;

    std::string urgentEmoji = StringHelpers::colorize("▲ ", RED);
    int urgentCount = 0;
    for (auto listName : listNames) {
        urgentCount += listItemService.countWithPriority(listName, { PriorityService::URGENT });
    }
    std::string urgentStr = std::to_string(urgentCount) + " ";
    int urgentCharLength = 2 + static_cast<int>(urgentStr.length());
    urgentStr = urgentEmoji + urgentStr;

    std::string highEmoji = StringHelpers::colorize("▶ ", ORANGE);
    int highCount = 0;
    for (auto listName : listNames) {
        highCount += listItemService.countWithPriority(listName, { PriorityService::HIGH });
    }
    std::string highStr = std::to_string(highCount) + " ";
    int highCharLength = 2 + static_cast<int>(highStr.length());
    highStr = highEmoji + highStr;

    std::string mediumEmoji = StringHelpers::colorize("▼ ", LIGHT_GREEN);
    int mediumCount = 0;
    for (auto listName : listNames) {
        mediumCount += listItemService.countWithPriority(listName, { PriorityService::MEDIUM });
    }
    std::string mediumStr = std::to_string(mediumCount) + " ";
    int mediumCharLength = 2 + static_cast<int>(mediumStr.length());
    mediumStr = mediumEmoji + mediumStr;

    std::string lowEmoji = StringHelpers::colorize("▽ ", GREEN);
    int lowCount = 0;
    for (auto listName : listNames) {
        lowCount += listItemService.countWithPriority(listName, { PriorityService::LOW });
    }
    std::string lowStr = std::to_string(lowCount) + " ";
    int lowCharLength = 2 + static_cast<int>(lowStr.length());
    lowStr = lowEmoji + lowStr;

    std::string priorityPrintCount = criticalStr + urgentStr + highStr + mediumStr + lowStr;
    int priorityCountLength = criticalCharLength + urgentCharLength + highCharLength + mediumCharLength + lowCharLength;

    int listTitleLength = consoleRowLength;
    int separator = listTitleLength - (statusCountLength + priorityCountLength);
    if (separator <= 10) {
        listTitleLength += STATUS_LENGTH;
        separator = listTitleLength - (statusCountLength + priorityCountLength);
    }
    if (separator <= 10) {
        statusPrintCount = totalStr + todoStr + startedStr + underReviewStr + pauseStr + completedStr + cancelStr +
                           archivedStr + deleteStr;
        statusCountLength = totalCharLength + todoCharLength + startedCharLength + pauseCharLength +
                            underReviewCharLength + completedCharLength + cancelCharLength + archivedCharLength +
                            deletedCharLength;
        separator = listTitleLength - (statusCountLength + priorityCountLength);
    }
    if (separator <= 10) {
        statusPrintCount = totalStr + todoStr + startedStr + underReviewStr + pauseStr + completedStr + cancelStr;
        statusCountLength = totalCharLength + todoCharLength + startedCharLength + pauseCharLength +
                            underReviewCharLength + completedCharLength + cancelCharLength;
        separator = listTitleLength - (statusCountLength + priorityCountLength);
    }
    if (separator <= 10) {
        statusPrintCount = totalStr + archivedStr;
        statusCountLength = totalCharLength + archivedCharLength;
        separator = listTitleLength - (statusCountLength + priorityCountLength);
    }
    if (separator <= 10) {
        statusPrintCount = totalStr;
        statusCountLength = totalCharLength;
        separator = listTitleLength - (statusCountLength + priorityCountLength);
    }
    if (separator <= 10) {
        priorityPrintCount = criticalCount + urgentCount;
        priorityCountLength = criticalCharLength + urgentCharLength;
        separator = listTitleLength - (statusCountLength + priorityCountLength);
    }
    if (separator <= 10) {
        priorityPrintCount = "";
        priorityCountLength = 0;
        separator = listTitleLength - (statusCountLength + priorityCountLength);
    }

    std::string showCount;
    showCount = statusPrintCount;
    showCount += StringHelpers::adjustStringLength("", separator);
    showCount += priorityPrintCount;

    if (currentListVariant == "archive") {
        std::string listNameTitle = StringHelpers::toUpper(titleListName + " archived");
        titleListName = StringHelpers::colorize(listNameTitle, LIGHT_YELLOW);
    } else if (currentListVariant == "delete") {
        std::string listNameTitle = StringHelpers::toUpper(titleListName + " deleted");
        titleListName = StringHelpers::colorize(listNameTitle, LIGHT_RED);
    } else {
        std::string listNameTitle = StringHelpers::toUpper(titleListName);
        titleListName = StringHelpers::colorize(listNameTitle, WHITE);
    }

    int listNameLength = static_cast<int>(StringHelpers::countCharsWithoutBashCodes(titleListName));
    int paddingLength = (listTitleLength - listNameLength) / 2;
    std::string paddingLeft =
        StringHelpers::colorize(StringHelpers::adjustStringLengthWithString("", paddingLength, "─"), GRAY);
    std::string paddingRight =
        StringHelpers::colorize(StringHelpers::adjustStringLengthWithString("", paddingLength, "─"), GRAY);

    if ((listTitleLength - listNameLength) % 2 != 0 || listNameLength % 2 != 0) {
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

void
MobileTheme::printATitle(std::string titleLine1, std::string titleLine2)
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
    ioService.print(StringHelpers::colorize(line4, WHITE));
    ioService.br();
}

void
MobileTheme::printStats(ListName& listName)
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
    ioService.printWithoutLineBreak(StringHelpers::adjustStringLength(" WHEN", STATS_WHEN_LENGTH));
    ioService.printWithoutLineBreak(StringHelpers::adjustStringLength("COMPL./CREAT.", STATS_CREATED_COMPLETED_LENGTH));
    ioService.printWithoutLineBreak(StringHelpers::adjustStringLength("PERC.", STATS_PERCENTAGE_LENGTH));
    ioService.br();
    printFullLine(GRAY);
    // today
    renderListStatLine(listName, DateHelpers::getTodayStart(), DateHelpers::getTodayEnd(), "today");
    // yesterday
    renderListStatLine(listName, DateHelpers::getTodayStart(-1), DateHelpers::getTodayEnd(-1), "yesterday");
    ioService.br();
    // Days of the week
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
    // This week
    renderListStatLine(listName, DateHelpers::getWeekStart(), DateHelpers::getWeekEnd(), "this week");
    // Last week
    renderListStatLine(listName, DateHelpers::getWeekStart(-1), DateHelpers::getWeekEnd(-1), "last week");
    ioService.br();
    // This month
    renderListStatLine(listName, DateHelpers::getMonthStart(), DateHelpers::getMonthEnd(), "this month");
    // Last month
    renderListStatLine(listName, DateHelpers::getMonthStart(-1), DateHelpers::getMonthEnd(-1), "last month");
    ioService.br();
    // This year
    renderListStatLine(listName, DateHelpers::getYearStart(), DateHelpers::getYearEnd(), "this year");
    // Since started
    renderListStatLine(listName, 0, time(nullptr), "since started");
}

void
MobileTheme::renderListStatLine(ListName& listName, time_t from, time_t to, std::string name)
{
    long created = listItemService.countCreatedBetween(listName, from, to);
    long completed = listItemService.countClosedBetween(listName, from, to);

    // percentage xx.xx% completed/created
    double percentage =
        completed > 0 ? std::floor((static_cast<double>(completed) / static_cast<double>(created)) * 10000) / 100 : 0;
    std::string percentageStr = std::to_string(percentage);
    percentageStr = percentageStr.substr(0, percentageStr.size() - 4);
    if (percentage >= 100) {
        percentageStr = StringHelpers::colorize(percentageStr, GREEN);
    } else if (percentage >= 50) {
        percentageStr = StringHelpers::colorize(percentageStr, LIGHT_YELLOW);
    } else if (percentage == 0) {
    } else {
        percentageStr = StringHelpers::colorize(percentageStr, RED);
    }

    std::string title = " ";
    title += StringHelpers::toUpper(name);
    title += ":";
    ioService.printWithoutLineBreak(StringHelpers::adjustStringLength(title, STATS_WHEN_LENGTH));
    ioService.printWithoutLineBreak(StringHelpers::adjustStringLength(
        std::to_string(completed) + "/" + std::to_string(created), STATS_CREATED_COMPLETED_LENGTH));
    ioService.printWithoutLineBreak(StringHelpers::adjustStringLength(percentageStr + "%", STATS_PERCENTAGE_LENGTH));
    ioService.br();
}

void
MobileTheme::printFullLine(std::string color)
{
    if (color.empty()) {
        color = GRAY;
    }

    std::string tmp =
        StringHelpers::colorize(StringHelpers::adjustStringLengthWithString("─", consoleWidth, "─"), color);
    ioService.print(tmp);
}

void
MobileTheme::printAListTitle(std::vector<std::string> titles, std::vector<int> titleSizes)
{
    std::string string;
    for (int i = 0; i < titles.size(); ++i) {
        if (titleSizes[i] > consoleRowLength) {
            titleSizes[i] = consoleRowLength;
        }
        string += StringHelpers::adjustStringLength(titles[i], titleSizes[i]);
    }

    ioService.print(string);
}

void
MobileTheme::printAList(std::vector<std::string> lines)
{
    for (const std::string& line : lines) {
        ioService.print(line);
    }
}
