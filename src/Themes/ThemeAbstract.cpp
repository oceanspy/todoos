#include "ThemeAbstract.h"
#include <cmath>

void
ThemeAbstract::print(ListName& listName, std::vector<ListItemEntity> listItems, bool showListName, bool showTitle)
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
ThemeAbstract::printMultipleList(std::vector<ListName>& listNames, std::vector<ListItemEntity>& listItems)
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
ThemeAbstract::buildId(const ListItemEntity& listItemEntity)
{
    if (*(*listItemEntity.status()).isClosed()) {
        return StringHelpers::colorize(" " + *listItemEntity.getId() + " ", GRAY);
    }
    return " " + *listItemEntity.getId() + " ";
}

std::string
ThemeAbstract::buildStatus(const ListItemEntity& listItemEntity)
{
    std::string render;
    std::string icon = *(*listItemEntity.status()).getIcon() + " ";
    render = !(*listItemEntity.status()).getStyle().empty()
                 ? StringHelpers::colorize(*(*listItemEntity.status()).getName(), (*listItemEntity.status()).getStyle())
                 : *(*listItemEntity.status()).getName();

    render = StringHelpers::adjustStringLength(icon + render, STATUS_LENGTH);
    std::string final_render = StringHelpers::colorize(render, (*listItemEntity.status()).getColor());

    return final_render;
}

std::string
ThemeAbstract::buildPriority(const ListItemEntity& listItemEntity)
{
    if (*(*listItemEntity.status()).isClosed()) {
        return StringHelpers::colorize(" " + *(*listItemEntity.priority()).getIcon() + "  ", GRAY);
    } else {
        return StringHelpers::colorize(" " + *(*listItemEntity.priority()).getIcon() + "  ",
                                       (*listItemEntity.priority()).getColor());
    }
}

std::string
ThemeAbstract::buildValue(const ListItemEntity& listItemEntity, const int leftOffset)
{
    std::string value = *listItemEntity.getValue();
    value = autoLineBreak(value, consoleRowLength - leftOffset, ID_LENGTH + PRIORITY_LENGTH + leftOffset);
    value = StringHelpers::colorize(value, (*listItemEntity.status()).getColor());
    value = StringHelpers::colorize(value, (*listItemEntity.status()).getStyle());
    return value;
}

std::string
ThemeAbstract::buildDate(const ListItemEntity& listItemEntity)
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
ThemeAbstract::printListName(std::vector<ListName>& listNames)
{
    std::string titleListName = "";
    for (auto listName : listNames) {
        titleListName += listName.getName() + " ";
    }
    titleListName.pop_back();

    std::string totalEmoji = "📈 ";
    int totalCount = 0;
    for (auto listName : listNames) {
        totalCount += listItemService.count(listName);
    }
    std::string totalStr = std::to_string(totalCount);
    int totalCharLength = 3 + static_cast<int>(totalStr.length());
    totalStr = totalEmoji + totalStr;

    std::string todoEmoji = " → ⏳ ";
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

    std::string pauseEmoji = " 💤 ";
    int pauseCount = 0;
    for (auto listName : listNames) {
        pauseCount += listItemService.countWithStatus(listName, { StatusService::PAUSED });
    }
    std::string pauseStr = std::to_string(pauseCount);
    int pauseCharLength = 4 + static_cast<int>(pauseStr.length());
    pauseStr = pauseEmoji + pauseStr;

    std::string blockedEmoji = " 🚫 ";
    int blockedCount = 0;
    for (auto listName : listNames) {
        blockedCount += listItemService.countWithStatus(listName, { StatusService::BLOCKED });
    }
    std::string blockedStr = std::to_string(blockedCount);
    int blockedCharLength = 4 + static_cast<int>(blockedStr.length());
    blockedStr = blockedEmoji + blockedStr;

    std::string archivedEmoji = " ↔ 🚀 ";
    int archivedCount = 0;
    for (auto listName : listNames) {
        ListName listNameArchive = ListName::createVariant(listName, "archive");
        archivedCount += listItemService.countWithStatus(listName, { StatusService::COMPLETED });
        archivedCount += listItemService.countWithStatus(listNameArchive, { StatusService::COMPLETED });
    }
    std::string archivedStr = std::to_string(archivedCount);
    int archivedCharLength = 6 + static_cast<int>(archivedStr.length());
    archivedStr = archivedEmoji + archivedStr;

    std::string cancelledArchivedEmoji = " ✖️ ";
    int cancelledCount = 0;
    for (auto listName : listNames) {
        ListName listNameArchive = ListName::createVariant(listName, "archive");
        cancelledCount += listItemService.countWithStatus(listName, { StatusService::CANCELLED });
        cancelledCount += listItemService.countWithStatus(listNameArchive, { StatusService::CANCELLED });
    }
    std::string cancelledStr = std::to_string(cancelledCount);
    int cancelledArchivedCharLength = 4 + static_cast<int>(cancelledStr.length());
    cancelledStr = cancelledArchivedEmoji + cancelledStr;

    std::string deletedEmoji = " 🧹 ";
    int deleteCount = 0;
    for (auto listName : listNames) {
        ListName listNameDelete = ListName::createVariant(listName, "delete");
        deleteCount += listItemService.count(listNameDelete);
    }
    std::string deleteStr = std::to_string(deleteCount);
    int deletedCharLength = 4 + static_cast<int>(deleteStr.length());
    deleteStr = deletedEmoji + deleteStr;

    std::string statusPrintCount = totalStr + todoStr + startedStr + underReviewStr + pauseStr + blockedStr +
                                   archivedStr + cancelledStr + deleteStr;
    int statusCountLength = totalCharLength + todoCharLength + startedCharLength + pauseCharLength + blockedCharLength +
                            underReviewCharLength + archivedCharLength + cancelledArchivedCharLength +
                            deletedCharLength;

    std::string criticalEmoji = StringHelpers::colorize("■ ", WHITE);
    int criticalCount = 0;
    for (auto listName : listNames) {
        criticalCount += listItemService.countWithPriority(listName, { PriorityService::CRITICAL });
    }
    std::string criticalStr = std::to_string(criticalCount) + " ";
    int criticalCharLength = 2 + static_cast<int>(criticalStr.length());
    criticalStr = criticalEmoji + criticalStr;

    std::string urgentEmoji = StringHelpers::colorize("● ", RED);
    int urgentCount = 0;
    for (auto listName : listNames) {
        urgentCount += listItemService.countWithPriority(listName, { PriorityService::URGENT });
    }
    std::string urgentStr = std::to_string(urgentCount) + " ";
    int urgentCharLength = 2 + static_cast<int>(urgentStr.length());
    urgentStr = urgentEmoji + urgentStr;

    std::string highEmoji = StringHelpers::colorize("● ", ORANGE);
    int highCount = 0;
    for (auto listName : listNames) {
        highCount += listItemService.countWithPriority(listName, { PriorityService::HIGH });
    }
    std::string highStr = std::to_string(highCount) + " ";
    int highCharLength = 2 + static_cast<int>(highStr.length());
    highStr = highEmoji + highStr;

    std::string mediumEmoji = StringHelpers::colorize("● ", LIGHT_GREEN);
    int mediumCount = 0;
    for (auto listName : listNames) {
        mediumCount += listItemService.countWithPriority(listName, { PriorityService::MEDIUM });
    }
    std::string mediumStr = std::to_string(mediumCount) + " ";
    int mediumCharLength = 2 + static_cast<int>(mediumStr.length());
    mediumStr = mediumEmoji + mediumStr;

    std::string lowEmoji = StringHelpers::colorize("◌ ", GREEN);
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
        statusPrintCount = totalStr + todoStr + startedStr + underReviewStr + pauseStr + archivedStr + deleteStr;
        statusCountLength = totalCharLength + todoCharLength + startedCharLength + pauseCharLength +
                            underReviewCharLength + archivedCharLength + deletedCharLength;
        separator = listTitleLength - (statusCountLength + priorityCountLength);
    }
    if (separator <= 10) {
        statusPrintCount = totalStr + todoStr + startedStr + underReviewStr + pauseStr;
        statusCountLength =
            totalCharLength + todoCharLength + startedCharLength + pauseCharLength + underReviewCharLength;
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
ThemeAbstract::printFullLine(std::string color)
{
    if (color.empty()) {
        color = GRAY;
    }

    std::string tmp =
        StringHelpers::colorize(StringHelpers::adjustStringLengthWithString("─", consoleWidth, "─"), color);
    ioService.print(tmp);
}

void
ThemeAbstract::printAList(std::vector<std::string> lines)
{
    for (const std::string& line : lines) {
        ioService.print(line);
    }
}

void
ThemeAbstract::renderListStatLine(ListName& listName, time_t from, time_t to, std::string name)
{
    long created = listItemService.countCreatedBetween(listName, from, to);
    long completed = listItemService.countClosedBetween(listName, from, to);
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
    ioService.printWithoutLineBreak(StringHelpers::adjustStringLength(title, statsWhenLength));
    ioService.printWithoutLineBreak(StringHelpers::adjustStringLength(
        std::to_string(completed) + "/" + std::to_string(created), statsCreatedCompletedLength));
    ioService.printWithoutLineBreak(StringHelpers::adjustStringLength(percentageStr + "%", statsPercentageLength));
    ioService.br();
}

std::string
ThemeAbstract::autoLineBreak(const std::string& value, const int& length, const int leadingSpaces)
{
    std::string br = "\n";
    br += StringHelpers::adjustStringLength("", leadingSpaces);
    std::string currentLine;
    std::vector<std::string> lines;

    std::vector<std::string> words = splitString(value);

    for (int i = 0; i < words.size(); i++) {
        if (currentLine.length() + words[i].length() >= length) {
            if (words[i].length() > length) {
                int j = 0;
                while (j < words[i].length()) {
                    if (currentLine.length() >= length) {
                        currentLine += br;
                        lines.push_back(currentLine);
                        currentLine = "";
                    }

                    currentLine += words[i][j];

                    if (currentLine.length() >= length) {
                        currentLine += br;
                        lines.push_back(currentLine);
                        currentLine = "";
                    }
                    j++;
                }
                continue;
            }

            currentLine += br;
            lines.push_back(currentLine);
            currentLine = "";
        }
        currentLine += words[i] + " ";
    }

    lines.push_back(StringHelpers::adjustStringLength(currentLine, length));

    std::string result;
    for (int i = 0; i < lines.size(); i++) {
        result += lines[i];
    }
    return result;
}

std::vector<std::string>
ThemeAbstract::splitString(const std::string& str)
{
    std::istringstream iss(str);
    std::vector<std::string> tokens;

    std::copy(
        std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>(), std::back_inserter(tokens));

    return tokens;
}
