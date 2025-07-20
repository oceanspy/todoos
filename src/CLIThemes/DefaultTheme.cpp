#include "DefaultTheme.h"
#include <cmath>

DefaultTheme::DefaultTheme(IOService& ioService,
                           ListService& listService,
                           ListItemService& listItemService,
                           int consoleWidth,
                           int consoleRowMaxLength)
  : ThemeAbstract(ioService, listService, listItemService, consoleWidth)
  , consoleRowLength(consoleRowMaxLength - 56)
{
}

void
DefaultTheme::print(std::string currentListName,
                    std::string currentListVariantName,
                    std::vector<ListItemEntity> listItems,
                    bool showListName,
                    bool showTitle)
{
    currentList = std::move(currentListName);
    currentListVariant = std::move(currentListVariantName);

    if (listService.getType(currentList) != "default") {
        // TODO: Implement other list types
    }

    bool hideListNameInLine = false;
    if (showListName) {
        printListName();
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

std::string
DefaultTheme::buildTitle()
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
DefaultTheme::buildLine(const ListItemEntity& listItemEntity, bool hideListNameInLine)
{
    std::string line = "";
    // ID
    line += buildId(listItemEntity);
    // Priority
    line += buildPriority(listItemEntity);
    // Value
    if (!hideListNameInLine) {
        line += StringHelpers::adjustStringLength(*listItemEntity.getListName() + " ", LISTNAME_LENGTH);
    }
    line += buildValue(listItemEntity);
    // Status
    line += buildStatus(listItemEntity);
    // Date
    line += buildDate(listItemEntity);
    return line;
}

std::string
DefaultTheme::buildId(const ListItemEntity& listItemEntity)
{
    if (*(*listItemEntity.status()).isClosed()) {
        return StringHelpers::colorize(" " + *listItemEntity.getId() + " ", GRAY);
    }
    return " " + *listItemEntity.getId() + " ";
}

std::string
DefaultTheme::buildStatus(const ListItemEntity& listItemEntity)
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
DefaultTheme::buildPriority(const ListItemEntity& listItemEntity)
{
    if (*(*listItemEntity.status()).isClosed()) {
        return StringHelpers::colorize(" " + *(*listItemEntity.priority()).getIcon() + "  ", GRAY);
    } else {
        return StringHelpers::colorize(" " + *(*listItemEntity.priority()).getIcon() + "  ",
                                       (*listItemEntity.priority()).getColor());
    }
}

std::string
DefaultTheme::buildValue(const ListItemEntity& listItemEntity)
{
    std::string value = *listItemEntity.getValue();
    value = autoLineBreak(value, consoleRowLength, ID_LENGTH + PRIORITY_LENGTH);
    value = StringHelpers::colorize(value, (*listItemEntity.status()).getColor());
    value = StringHelpers::colorize(value, (*listItemEntity.status()).getStyle());
    return value;
}

std::string
DefaultTheme::buildDate(const ListItemEntity& listItemEntity)
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
DefaultTheme::printListName()
{
    std::string totalEmoji = "📄 ";
    std::string total = std::to_string(listItemService.load().count());
    int totalCharLength = 3 + static_cast<int>(total.length());
    total = totalEmoji + total;

    std::string todoEmoji = " ↔ ⏳ ";
    std::string todoCount = std::to_string(listItemService.load().countWithStatus({ StatusService::TO_DO }));
    int todoCharLength = 6 + static_cast<int>(todoCount.length());
    todoCount = todoEmoji + todoCount;

    std::string startedEmoji = " 🏃 ";
    std::string startedCount = std::to_string(listItemService.load().countWithStatus({ StatusService::STARTED }));
    int startedCharLength = 4 + static_cast<int>(startedCount.length());
    startedCount = startedEmoji + startedCount;

    std::string underReviewEmoji = " 🔍 ";
    std::string underReviewCount = std::to_string(listItemService.load().countWithStatus({ StatusService::REVIEWING }));
    int underReviewCharLength = 4 + static_cast<int>(underReviewCount.length());
    underReviewCount = underReviewEmoji + underReviewCount;

    std::string pauseEmoji = " 🚧 ";
    std::string pauseCount =
        std::to_string(listItemService.load().countWithStatus({ StatusService::PAUSED, StatusService::PENDING }));
    int pauseCharLength = 4 + static_cast<int>(pauseCount.length());
    pauseCount = pauseEmoji + pauseCount;

    std::string completedEmoji = " ✅ ";
    std::string completedCount = std::to_string(listItemService.load().countWithStatus({ StatusService::COMPLETED }));
    int completedCharLength = 4 + static_cast<int>(completedCount.length());
    completedCount = completedEmoji + completedCount;

    std::string cancelEmoji = " 🪧 ";
    std::string cancelCount = std::to_string(listItemService.load().countWithStatus({ StatusService::CANCELLED }));
    int cancelCharLength = 4 + static_cast<int>(cancelCount.length());
    cancelCount = cancelEmoji + cancelCount;

    std::string archivedEmoji = " ▭ 🚀 ";
    std::string archivedCount =
        std::to_string(listItemService.loadVariant("archive").countWithStatus({ StatusService::COMPLETED }));
    int archivedCharLength = 6 + static_cast<int>(archivedCount.length());
    archivedCount = archivedEmoji + archivedCount;

    std::string cancelledArchivedEmoji = " 🚫 ";
    std::string cancelledArchivedCount =
        std::to_string(listItemService.loadVariant("archive").countWithStatus({ StatusService::CANCELLED }));
    int cancelledArchivedCharLength = 4 + static_cast<int>(cancelledArchivedCount.length());
    cancelledArchivedCount = cancelledArchivedEmoji + cancelledArchivedCount;

    std::string deletedEmoji = " ↔ 🧹 ";
    std::string deletedCount = std::to_string(listItemService.loadVariant("delete").count());
    int deletedCharLength = 6 + static_cast<int>(deletedCount.length());
    deletedCount = deletedEmoji + deletedCount;

    std::string statusPrintCount = total + todoCount + startedCount + underReviewCount + pauseCount + completedCount +
                                   cancelCount + archivedCount + cancelledArchivedCount + deletedCount;
    int statusCountLength = totalCharLength + todoCharLength + startedCharLength + pauseCharLength +
                            underReviewCharLength + completedCharLength + cancelCharLength + archivedCharLength +
                            cancelledArchivedCharLength + deletedCharLength;

    std::string criticalEmoji = StringHelpers::colorize("■ ", WHITE);
    std::string criticalCount =
        std::to_string(listItemService.load().countWithPriority({ PriorityService::CRITICAL })) + " ";
    int criticalCharLength = 2 + static_cast<int>(criticalCount.length());
    criticalCount = criticalEmoji + criticalCount;

    std::string urgentEmoji = StringHelpers::colorize("▲ ", RED);
    std::string urgentCount =
        std::to_string(listItemService.load().countWithPriority({ PriorityService::URGENT })) + " ";
    int urgentCharLength = 2 + static_cast<int>(urgentCount.length());
    urgentCount = urgentEmoji + urgentCount;

    std::string highEmoji = StringHelpers::colorize("▶ ", ORANGE);
    std::string highCount = std::to_string(listItemService.load().countWithPriority({ PriorityService::HIGH })) + " ";
    int highCharLength = 2 + static_cast<int>(highCount.length());
    highCount = highEmoji + highCount;

    std::string mediumEmoji = StringHelpers::colorize("▼ ", LIGHT_GREEN);
    std::string mediumCount =
        std::to_string(listItemService.load().countWithPriority({ PriorityService::MEDIUM })) + " ";
    int mediumCharLength = 2 + static_cast<int>(mediumCount.length());
    mediumCount = mediumEmoji + mediumCount;

    std::string lowEmoji = StringHelpers::colorize("▽ ", GREEN);
    std::string lowCount = std::to_string(listItemService.load().countWithPriority({ PriorityService::LOW }));
    int lowCharLength = 2 + static_cast<int>(lowCount.length());
    lowCount = lowEmoji + lowCount;

    std::string priorityPrintCount = criticalCount + urgentCount + highCount + mediumCount + lowCount;
    int priorityCountLength = criticalCharLength + urgentCharLength + highCharLength + mediumCharLength + lowCharLength;

    int listTitleLength = consoleRowLength;
    int separator = listTitleLength - (statusCountLength + priorityCountLength);
    if (separator <= 10) {
        listTitleLength += STATUS_LENGTH;
        separator = listTitleLength - (statusCountLength + priorityCountLength);
    }
    if (separator <= 10) {
        statusPrintCount = total + todoCount + startedCount + underReviewCount + pauseCount + completedCount +
                           cancelCount + archivedCount + deletedCount;
        statusCountLength = totalCharLength + todoCharLength + startedCharLength + pauseCharLength +
                            underReviewCharLength + completedCharLength + cancelCharLength + archivedCharLength +
                            deletedCharLength;
        separator = listTitleLength - (statusCountLength + priorityCountLength);
    }
    if (separator <= 10) {
        statusPrintCount =
            total + todoCount + startedCount + underReviewCount + pauseCount + completedCount + cancelCount;
        statusCountLength = totalCharLength + todoCharLength + startedCharLength + pauseCharLength +
                            underReviewCharLength + completedCharLength + cancelCharLength;
        separator = listTitleLength - (statusCountLength + priorityCountLength);
    }
    if (separator <= 10) {
        statusPrintCount = total + archivedCount;
        statusCountLength = totalCharLength + archivedCharLength;
        separator = listTitleLength - (statusCountLength + priorityCountLength);
    }
    if (separator <= 10) {
        statusPrintCount = total;
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

    std::string listName;
    if (currentListVariant == "archive") {
        std::string listNameTitle = StringHelpers::toUpper(currentList + " archived");
        listName = StringHelpers::colorize(listNameTitle, LIGHT_YELLOW);
    } else if (currentListVariant == "delete") {
        std::string listNameTitle = StringHelpers::toUpper(currentList + " deleted");
        listName = StringHelpers::colorize(listNameTitle, LIGHT_RED);
    } else {
        std::string listNameTitle = StringHelpers::toUpper(currentList);
        listName = StringHelpers::colorize(listNameTitle, WHITE);
    }

    int listNameLength = static_cast<int>(StringHelpers::countCharsWithoutBashCodes(listName));
    int paddingLength = (listTitleLength - listNameLength) / 2;
    std::string paddingLeft =
        StringHelpers::colorize(StringHelpers::adjustStringLengthWithString("", paddingLength, "─"), GRAY);
    std::string paddingRight =
        StringHelpers::colorize(StringHelpers::adjustStringLengthWithString("", paddingLength, "─"), GRAY);

    if ((listTitleLength - listNameLength) % 2 != 0 || listNameLength % 2 != 0) {
        listName += StringHelpers::colorize("─", GRAY);
    }

    std::string line1 = " ╔═══" + StringHelpers::adjustStringLengthWithString("═", listTitleLength, "═") + "═══╗";
    std::string line2 = " ║   " + paddingLeft + listName + paddingRight + "   ║";
    std::string line3 = " ║   " + showCount + "   ║";
    std::string line4 = " ╚═══" + StringHelpers::adjustStringLengthWithString("═", listTitleLength, "═") + "═══╝";

    ioService.print(StringHelpers::colorize(line1, WHITE));
    ioService.print(line2);
    ioService.print(line3);
    ioService.print(StringHelpers::colorize(line4, WHITE));
    ioService.br();
}

void
DefaultTheme::printATitle(std::string titleLine1, std::string titleLine2)
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
    if (!line2.empty()) {
        ioService.print(line3);
    }
    ioService.print(StringHelpers::colorize(line4, WHITE));
    ioService.br();
}

void
DefaultTheme::printStats()
{
    std::string totalEmoji = " 📄 Total: ";
    std::string total = std::to_string(listItemService.load().count());
    total = totalEmoji + total;

    std::string todoEmoji = " ⏳ To-Do: ";
    std::string todoCount = std::to_string(listItemService.load().countWithStatus({ StatusService::TO_DO }));
    todoCount = todoEmoji + todoCount;

    std::string startedEmoji = " 🏃 Started: ";
    std::string startedCount = std::to_string(listItemService.load().countWithStatus({ StatusService::STARTED }));
    startedCount = startedEmoji + startedCount;

    std::string underReviewEmoji = " 🔍 Reviewing: ";
    std::string underReviewCount = std::to_string(listItemService.load().countWithStatus({ StatusService::REVIEWING }));
    underReviewCount = underReviewEmoji + underReviewCount;

    std::string pauseEmoji = " 🚧 Paused: ";
    std::string pauseCount = std::to_string(listItemService.load().countWithStatus({ StatusService::PAUSED }));
    pauseCount = pauseEmoji + pauseCount;

    std::string completedEmoji = " ✅ Completed: ";
    std::string completedCount = std::to_string(listItemService.load().countWithStatus({ StatusService::COMPLETED }));
    completedCount = completedEmoji + completedCount;

    std::string cancelEmoji = " 🪧 Cancelled: ";
    std::string cancelCount = std::to_string(listItemService.load().countWithStatus({ StatusService::CANCELLED }));
    cancelCount = cancelEmoji + cancelCount;

    std::string archivedEmoji = " 🚀 Archived: ";
    std::string archivedCount = std::to_string(listItemService.loadVariant("archive").count());
    archivedCount = archivedEmoji + archivedCount;

    std::string cancelledArchivedEmoji = "  🚫 Cancelled: ";
    std::string cancelledArchivedCount =
        std::to_string(listItemService.loadVariant("archive").countWithStatus({ StatusService::CANCELLED }));
    cancelledArchivedCount = cancelledArchivedEmoji + cancelledArchivedCount;

    std::string deletedEmoji = "  🧹 Deleted: ";
    std::string deletedCount = std::to_string(listItemService.loadVariant("delete").count());
    deletedCount = deletedEmoji + deletedCount;

    std::string criticalEmoji = StringHelpers::colorize(" ■ ", WHITE) + "Critical: ";
    std::string criticalCount =
        std::to_string(listItemService.load().countWithPriority({ PriorityService::CRITICAL })) + " ";
    criticalCount = criticalEmoji + criticalCount;

    std::string urgentEmoji = StringHelpers::colorize(" ▲ ", RED) + "Urgent: ";
    std::string urgentCount =
        std::to_string(listItemService.load().countWithPriority({ PriorityService::URGENT })) + " ";
    urgentCount = urgentEmoji + urgentCount;

    std::string highEmoji = StringHelpers::colorize(" ▶ ", ORANGE) + "High: ";
    std::string highCount = std::to_string(listItemService.load().countWithPriority({ PriorityService::HIGH })) + " ";
    highCount = highEmoji + highCount;

    std::string mediumEmoji = StringHelpers::colorize(" ▼ ", LIGHT_GREEN) + "Medium: ";
    std::string mediumCount =
        std::to_string(listItemService.load().countWithPriority({ PriorityService::MEDIUM })) + " ";
    mediumCount = mediumEmoji + mediumCount;

    std::string lowEmoji = StringHelpers::colorize(" ▽ ", GREEN) + "Low: ";
    std::string lowCount = std::to_string(listItemService.load().countWithPriority({ PriorityService::LOW }));
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
    renderListStatLine(DateHelpers::getTodayStart(), DateHelpers::getTodayEnd(), "today");
    // yesterday
    renderListStatLine(DateHelpers::getTodayStart(-1), DateHelpers::getTodayEnd(-1), "yesterday");
    ioService.br();
    // Days of the week
    renderListStatLine(
        DateHelpers::getDayStart("monday", time(nullptr)), DateHelpers::getDayEnd("monday", time(nullptr)), "monday");
    renderListStatLine(DateHelpers::getDayStart("tuesday", time(nullptr)),
                       DateHelpers::getDayEnd("tuesday", time(nullptr)),
                       "tuesday");
    renderListStatLine(DateHelpers::getDayStart("wednesday", time(nullptr)),
                       DateHelpers::getDayEnd("wednesday", time(nullptr)),
                       "wednesday");
    renderListStatLine(DateHelpers::getDayStart("thursday", time(nullptr)),
                       DateHelpers::getDayEnd("thursday", time(nullptr)),
                       "thursday");
    renderListStatLine(
        DateHelpers::getDayStart("friday", time(nullptr)), DateHelpers::getDayEnd("friday", time(nullptr)), "friday");
    renderListStatLine(DateHelpers::getDayStart("saturday", time(nullptr)),
                       DateHelpers::getDayEnd("saturday", time(nullptr)),
                       "saturday");
    renderListStatLine(
        DateHelpers::getDayStart("sunday", time(nullptr)), DateHelpers::getDayEnd("sunday", time(nullptr)), "sunday");
    ioService.br();
    // This week
    renderListStatLine(DateHelpers::getWeekStart(), DateHelpers::getWeekEnd(), "this week");
    // Last week
    renderListStatLine(DateHelpers::getWeekStart(-1), DateHelpers::getWeekEnd(-1), "last week");
    ioService.br();
    // This month
    renderListStatLine(DateHelpers::getMonthStart(), DateHelpers::getMonthEnd(), "this month");
    // Last month
    renderListStatLine(DateHelpers::getMonthStart(-1), DateHelpers::getMonthEnd(-1), "last month");
    ioService.br();
    // This year
    renderListStatLine(DateHelpers::getYearStart(), DateHelpers::getYearEnd(), "this year");
    // Since started
    renderListStatLine(0, time(nullptr), "since started");
}

void
DefaultTheme::renderListStatLine(time_t from, time_t to, std::string name)
{
    long created = listItemService.load().countCreatedBetween(from, to);
    long completed = listItemService.load().countClosedBetween(from, to);
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
DefaultTheme::printFullLine(std::string color)
{
    if (color.empty()) {
        color = GRAY;
    }

    std::string tmp =
        StringHelpers::colorize(StringHelpers::adjustStringLengthWithString("─", consoleWidth, "─"), color);
    ioService.print(tmp);
}

void
DefaultTheme::printAList(std::vector<std::string> lines)
{
    for (const std::string& line : lines) {
        ioService.print(line);
    }
}

void
DefaultTheme::printAListTitle(std::vector<std::string> titles, std::vector<int> titleSizes)
{
    std::string string;
    for (int i = 0; i < titles.size(); ++i) {
        string += StringHelpers::adjustStringLength(titles[i], titleSizes[i]);
    }

    ioService.print(string);
}
