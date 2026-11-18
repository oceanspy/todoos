#include "Theme.h"
#include <cmath>

void
Theme::print(ListName& listName, std::vector<ListItemEntity> listItems, bool showListName, bool showTitle)
{
    fitIdColumn(listItems);

    bool hideListNameInLine = false;
    if (showListName) {
        printListTitle(listName);
        hideListNameInLine = true;
    }

    if (showTitle) {
        std::string title = printListTitleRow();
        ioService.print(title);
        printFullLine(GRAY);
    }

    if (listItems.empty() && showTitle) {
        ioService.print(" No items found.");
        return;
    }

    for (size_t i = 0; i < listItems.size(); i++) {
        childConnector = childConnectorFor(listItems, i);
        std::string line = printListRow(listItems[i], hideListNameInLine);
        ioService.print(line);
    }
    childConnector = "";
}

void
Theme::printMultipleList(std::vector<ListName>& listNames, std::vector<ListItemEntity>& listItems)
{
    printMultipleListTitles(listNames);
    fitIdColumn(listItems);

    std::string title = printListTitleRow();
    ioService.print(title);
    printFullLine(GRAY);

    if (listItems.empty()) {
        ioService.print(" No items found.");
        return;
    }

    for (size_t i = 0; i < listItems.size(); i++) {
        childConnector = childConnectorFor(listItems, i);
        std::string line = printListRow(listItems[i], false);
        ioService.print(line);
    }
    childConnector = "";
}

std::string
Theme::childConnectorFor(const std::vector<ListItemEntity>& listItems, size_t index)
{
    const ListItemEntity& item = listItems[index];
    if (!item.hasParent()) {
        return "";
    }

    // Shown under its parent: walking up through its siblings reaches the parent row
    const std::string parentKey = ListItemService::itemKey(item, *item.getParentId());
    auto isSibling = [&parentKey](const ListItemEntity& row) {
        return row.hasParent() && ListItemService::itemKey(row, *row.getParentId()) == parentKey;
    };

    size_t j = index;
    while (j > 0 && isSibling(listItems[j - 1])) {
        j--;
    }
    if (j == 0 || ListItemService::itemKey(listItems[j - 1], *listItems[j - 1].getId()) != parentKey) {
        return "";
    }

    const bool isLast = index + 1 >= listItems.size() || !isSibling(listItems[index + 1]);
    return isLast ? "└─ " : "├─ ";
}

void
Theme::fitIdColumn(const std::vector<ListItemEntity>& listItems)
{
    idColumnLength = ID_LENGTH;
    for (const ListItemEntity& listItemEntity : listItems) {
        int length = static_cast<int>((*listItemEntity.getId()).length()) + 2;
        if (length > idColumnLength) {
            idColumnLength = length;
        }
    }
}

std::string
Theme::buildId(const ListItemEntity& listItemEntity)
{
    std::string id = StringHelpers::adjustStringLength(" " + *listItemEntity.getId() + " ", idColumnLength);
    if (*(*listItemEntity.status()).isClosed()) {
        return StringHelpers::colorize(id, GRAY);
    }
    return id;
}

std::string
Theme::buildStatus(const ListItemEntity& listItemEntity)
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
Theme::buildPriority(const ListItemEntity& listItemEntity)
{
    std::string icon = *(*listItemEntity.priority()).getIcon();
    if (*listItemEntity.hasDescription()) {
        icon = "⇱";
    }
    if (*(*listItemEntity.status()).isClosed()) {
        return StringHelpers::colorize(" " + icon + "  ", GRAY);
    } else {
        return StringHelpers::colorize(" " + icon + "  ", (*listItemEntity.priority()).getColor());
    }
}

std::string
Theme::buildValue(const ListItemEntity& listItemEntity, const int leftOffset)
{
    auto colorizeValue = [&listItemEntity](const std::string& text) {
        return StringHelpers::colorize(StringHelpers::colorize(text, (*listItemEntity.status()).getColor()),
                                       (*listItemEntity.status()).getStyle());
    };

    std::string value = *listItemEntity.getValue();
    if (childConnector.empty()) {
        value = autoLineBreak(value,
                              consoleRowLength - leftOffset - idColumnExtraLength(),
                              idColumnLength + PRIORITY_LENGTH + leftOffset);
        return colorizeValue(value);
    }

    // Child row: the connector takes the first columns of the value, wrapped lines line up under the
    // child's text and keep the vertical line while more children follow
    const int leadingSpaces = idColumnLength + PRIORITY_LENGTH + leftOffset + CHILD_CONNECTOR_LENGTH;
    value = autoLineBreak(
        value, consoleRowLength - leftOffset - idColumnExtraLength() - CHILD_CONNECTOR_LENGTH, leadingSpaces);

    const std::string lineBreak = "\n" + StringHelpers::adjustStringLength("", leadingSpaces);
    const std::string continuation = "\n" + StringHelpers::adjustStringLength("", leadingSpaces - CHILD_CONNECTOR_LENGTH) +
                                     StringHelpers::colorize(childConnector == "├─ " ? "│  " : "   ", GRAY);
    std::string render = StringHelpers::colorize(childConnector, GRAY);
    size_t start = 0;
    size_t found;
    while ((found = value.find(lineBreak, start)) != std::string::npos) {
        render += colorizeValue(value.substr(start, found - start)) + continuation;
        start = found + lineBreak.size();
    }
    render += colorizeValue(value.substr(start));
    return render;
}

std::string
Theme::buildDate(const ListItemEntity& listItemEntity)
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
Theme::printFullLine(std::string color)
{
    if (color.empty()) {
        color = GRAY;
    }

    std::string tmp =
        StringHelpers::colorize(StringHelpers::adjustStringLengthWithString("─", consoleWidth, "─"), color);
    ioService.print(tmp);
}

void
Theme::printAList(std::vector<std::string> lines)
{
    for (const std::string& line : lines) {
        ioService.print(line);
    }
}

void
Theme::renderListStatLine(ListName& listName, time_t from, time_t to, std::string name)
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
Theme::autoLineBreak(const std::string& value, const int& length, const int leadingSpaces)
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

std::string
Theme::buildPriorityCounts(const ListCountSummary& summary)
{
    return StringHelpers::colorize("■ ", WHITE) + std::to_string(summary.getPriority(PriorityService::CRITICAL)) + " " +
           StringHelpers::colorize("● ", RED) + std::to_string(summary.getPriority(PriorityService::URGENT)) + " " +
           StringHelpers::colorize("● ", ORANGE) + std::to_string(summary.getPriority(PriorityService::HIGH)) + " " +
           StringHelpers::colorize("● ", LIGHT_GREEN) + std::to_string(summary.getPriority(PriorityService::MEDIUM)) +
           " " + StringHelpers::colorize("◌ ", GREEN) + std::to_string(summary.getPriority(PriorityService::LOW));
}

std::string
Theme::buildShortStatsCounts(const ListCountSummary& summary)
{
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
    cancelledStr = " 🚫 " + cancelledStr;

    std::string deletedStr = std::to_string(summary.deleted);
    int deletedCharLength = 4 + static_cast<int>(deletedStr.length());
    deletedStr = " 🗑️ " + deletedStr;

    std::string statusPrintCount = totalStr + archivedStr + deliveredStr + cancelledStr + deletedStr;

    return statusPrintCount;
}

std::vector<std::string>
Theme::splitString(const std::string& str)
{
    std::istringstream iss(str);
    std::vector<std::string> tokens;

    std::copy(
        std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>(), std::back_inserter(tokens));

    return tokens;
}
