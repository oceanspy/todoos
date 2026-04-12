#include "Theme.h"
#include <cmath>

void
Theme::print(ListName& listName, std::vector<ListItemEntity> listItems, bool showListName, bool showTitle)
{
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

    for (const ListItemEntity& listItemEntity : listItems) {
        std::string line = printListRow(listItemEntity, hideListNameInLine);
        ioService.print(line);
    }
}

void
Theme::printMultipleList(std::vector<ListName>& listNames, std::vector<ListItemEntity>& listItems)
{
    std::string title = printListTitleRow();
    ioService.print(title);
    printFullLine(GRAY);

    if (listItems.empty()) {
        ioService.print(" No items found.");
        return;
    }

    for (const ListItemEntity& listItemEntity : listItems) {
        std::string line = printListRow(listItemEntity, false);
        ioService.print(line);
    }
}

std::string
Theme::buildId(const ListItemEntity& listItemEntity)
{
    if (*(*listItemEntity.status()).isClosed()) {
        return StringHelpers::colorize(" " + *listItemEntity.getId() + " ", GRAY);
    }
    return " " + *listItemEntity.getId() + " ";
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
    if (*(*listItemEntity.status()).isClosed()) {
        return StringHelpers::colorize(" " + *(*listItemEntity.priority()).getIcon() + "  ", GRAY);
    } else {
        return StringHelpers::colorize(" " + *(*listItemEntity.priority()).getIcon() + "  ",
                                       (*listItemEntity.priority()).getColor());
    }
}

std::string
Theme::buildValue(const ListItemEntity& listItemEntity, const int leftOffset)
{
    std::string value = *listItemEntity.getValue();
    value = autoLineBreak(value, consoleRowLength - leftOffset, ID_LENGTH + PRIORITY_LENGTH + leftOffset);
    value = StringHelpers::colorize(value, (*listItemEntity.status()).getColor());
    value = StringHelpers::colorize(value, (*listItemEntity.status()).getStyle());
    return value;
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

std::vector<std::string>
Theme::splitString(const std::string& str)
{
    std::istringstream iss(str);
    std::vector<std::string> tokens;

    std::copy(
        std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>(), std::back_inserter(tokens));

    return tokens;
}
