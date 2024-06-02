#include "DateHelpers.h"

#include <utility>

std::string DateHelpers::formatTimestamp(time_t timestamp, std::string format, const std::string& separator)
{
    if (timestamp == 0) {
        return "N/A";
    }

    if (format == "default") {
        format = "%d" + separator + "%m" + separator + "%Y - %H:%M";
    }
    else if (format == "date" || format == "dmy") {
        format = "%d" + separator + "%m" + separator + "%Y";
    }
    else if (format == "ymd") {
        format = "%Y" + separator + "%m" + separator + "%d";
    }
    else if (format == "shortTime") {
        format = "%H:%M";
    }
    else if (format == "time") {
        format = "%H:%M:%S";
    }
    else if (format == "long") {
        format = "%d" + separator + "%m" + separator + "%Y - %H:%M:%S";
    } else if (format == "d" || format == "day") {
        format = "%d";
    } else if (format == "m" || format == "month") {
        format = "%m";
    } else if (format == "y" || format == "year") {
        format = "%Y";
    }

    std::stringstream ss;
    tm* timeinfo = localtime(&timestamp);
    ss << std::put_time(timeinfo, format.c_str());
    return ss.str();
}

std::string DateHelpers::formatTimestampToHumanDate(time_t timestamp, std::string format)
{
    if (timestamp == 0) {
        return "N/A";
    }

    std::stringstream ss;
    tm* timeinfo = localtime(&timestamp);
    if (format == "default") {
//        ss << std::put_time(timeinfo, "%a %d.%m at %H:%M");
        ss << std::put_time(timeinfo, "%a %d %b at %H:%M");
    }
    else if (format == "date") {
        ss << std::put_time(timeinfo, "%a %d %b");
    }
    else {
        ss << std::put_time(timeinfo, "%A %eth at %H:%M");
    }
    return ss.str();
}

bool DateHelpers::isTimestampToday(time_t timestamp) {
    // Get the current time
    time_t now = time(nullptr);
    tm *now_tm = localtime(&now);

    // Set the hour, minute, and second fields to 0
    now_tm->tm_hour = 0;
    now_tm->tm_min = 0;
    now_tm->tm_sec = 0;

    // Convert the tm structure back to a timestamp
    time_t startOfDay = mktime(now_tm);

    // Compare the given timestamp with the start of the current day
    return timestamp >= startOfDay;
}

bool DateHelpers::isTimestampNDaysFromToday(time_t timestamp, int n) {
    // Get the current time
    time_t now = time(nullptr);
    tm *now_tm = localtime(&now);

    // Set the hour, minute, and second fields to 0
    now_tm->tm_hour = 0;
    now_tm->tm_min = 0;
    now_tm->tm_sec = 0;

    // Add or subtract the number of days
    now_tm->tm_mday += n;

    // Convert the tm structure back to a timestamp
    time_t startOfTargetDay = mktime(now_tm);

    // Get the start of the next day
    now_tm->tm_mday += 1;
    time_t startOfNextDay = mktime(now_tm);

    // Compare the given timestamp with the start of the target day
    return timestamp >= startOfTargetDay && timestamp < startOfNextDay;
}

std::string DateHelpers::timestampToDuration(time_t startedAt, time_t endedAt)
{
    time_t timestamp = (int) difftime(endedAt, startedAt);
    std::stringstream ss;
    int years = timestamp / 31536000;
    int days = timestamp / 86400;
    int hours = (timestamp % 86400) / 3600;
    int minutes = (timestamp % 3600) / 60;
    int seconds = timestamp % 60;

    if (years > 0) {
        ss << years << "y";
        return "> " + ss.str();
    }
    if (days > 0) {
        ss << days << "d ";
    }
    if (years == 0 && hours > 0) {
        ss << hours << "h ";
    }
    if (years == 0 && days < 99 && minutes > 0) {
        ss << minutes << "m ";
    }
    if (years == 0 && days < 99 && seconds > 0) {
        ss << seconds << "s";
    }

    // remove trailing space if there is one
    if (ss.str().back() == ' ') {
        ss.str(ss.str().substr(0, ss.str().size() - 1));
    }

    return ss.str();
}

time_t DateHelpers::relativeDateToTimestamp(std::string stringDate, time_t referenceTime)
{
    // TODO: refactor this method to be more readable
    // -> Also create an Helper dedicated to date manipulation
    if (stringDate.empty()) {
        return 0;
    }

    if (stringDate == "reset" || stringDate == "clear" || stringDate == "none") {
        return 0;
    }

    if (stringDate == "now") {
        return std::time(nullptr);
    }

    if (stringDate == "end-of-day") {
        return endOfDayTimestamp(std::time(nullptr));
    }

    std::string returnDate;

    std::time_t t;
    if (referenceTime == 0) {
        t = std::time(nullptr);   // get time now
    } else {
        t = referenceTime;
    }

    // Weekdays
    std::map<std::string, int> weekDays = {
        {"sunday", 0}, {"monday", 1}, {"tuesday", 2}, {"wednesday", 3},
        {"thursday", 4}, {"friday", 5}, {"saturday", 6}
    };

    // if stringDate is a recognized word
    if (returnDate.empty() && weekDays.count(StringHelpers::toLower(stringDate)) > 0)
    {
        std::tm* now = std::localtime(&t);
        int currentDayOfWeek = now->tm_wday;
        int targetDayOfWeek = weekDays[StringHelpers::toLower(stringDate)];
        int daysUntilTarget = (targetDayOfWeek - currentDayOfWeek + 7) % 7;
        if (daysUntilTarget == 0) { // If today is the target day, set to next week
            daysUntilTarget = 7;
        }
        now->tm_mday += daysUntilTarget;
        mktime(now); // Normalize the date
        returnDate = std::to_string(now->tm_year + 1900) + "-" + std::to_string(now->tm_mon + 1) + "-" + std::to_string(now->tm_mday);
    }

    // if stringDate is a recognized word
    if (returnDate.empty() && stringDate == "today")
    {
        std::tm* now = std::localtime(&t);
        returnDate = std::to_string(now->tm_year + 1900) + "-" + std::to_string(now->tm_mon + 1) + "-" + std::to_string(now->tm_mday);
    }
    else if (returnDate.empty() && stringDate == "tomorrow")
    {
        std::tm* now = std::localtime(&t);
        now->tm_mday += 1; // Add 7 days to the current date
        std::mktime(now); // Normalize the date
        returnDate = std::to_string(now->tm_year + 1900) + "-" + std::to_string(now->tm_mon + 1) + "-" + std::to_string(now->tm_mday);
    }
    else if (returnDate.empty() && stringDate == "next-week")
    {
        std::tm* now = std::localtime(&t);
        now->tm_mday += 7; // Add 7 days to the current date
        std::mktime(now); // Normalize the date
        returnDate = std::to_string(now->tm_year + 1900) + "-" + std::to_string(now->tm_mon + 1) + "-" + std::to_string(now->tm_mday);
    }
    else if (returnDate.empty() && stringDate == "next-month")
    {
        std::tm* now = std::localtime(&t);
        std::mktime(now); // Normalize the date
        returnDate = std::to_string(now->tm_year + 1900) + "-" + std::to_string(now->tm_mon + 1) + "-" + std::to_string(now->tm_mday);
    }
    else if (returnDate.empty() && stringDate == "next-year")
    {
        std::tm* now = std::localtime(&t);
        std::mktime(now); // Normalize the date
        returnDate = std::to_string(now->tm_year + 1901) + "-" + std::to_string(now->tm_mon + 1) + "-" + std::to_string(now->tm_mday);
    }

    // if date format is "xxxx.xx.xx"
    if (returnDate.empty() && stringDate.find('.') != std::string::npos)
    {
        std::replace(stringDate.begin(), stringDate.end(), '.', '-');
        returnDate = stringDate;
    }

    // if date format is "xx-xx"
    if (returnDate.empty() && stringDate.length() == 5 && (stringDate.find('.') != std::string::npos || stringDate.find('-') != std::string::npos))
    {
        std::tm* now = std::localtime(&t);
        int year = now->tm_year + 1900; // tm_year is years since 1900
        returnDate = std::to_string(year) + "-" + stringDate;
    }

    // if date format is "xd", "xm", "xy", "xday(s)", "xmonth(s)", "xyear(s)"
    if (returnDate.empty())
    {
        std::string unit;
        char lastChar;
        if (stringDate.size() >= 3) {
            unit = stringDate.substr(stringDate.size() - 3);
        }
        if (stringDate.size() >= 4 && (unit == "day" || unit == "ays")) {
            stringDate.erase(stringDate.size() - (unit == "day" ? 2 : 3));
            lastChar = 'd';
        } else if (stringDate.size() >= 5 && (unit == "nth" || unit == "ths")) {
            stringDate.erase(stringDate.size() - (unit == "onth" ? 3 : 4));
            lastChar = 'm';
        } else if (stringDate.size() >= 4 && (unit == "ear" || unit == "ars")) {
            stringDate.erase(stringDate.size() - (unit == "year" ? 2 : 3));
            lastChar = 'y';
        } else {
            lastChar = stringDate.back();
        }


        if (lastChar == 'd' || lastChar == 'm' || lastChar == 'y') {
            std::string numberPart = stringDate.substr(0, stringDate.size() - 1);
            int number = std::stoi(numberPart);

            std::tm* now = std::localtime(&t);

            if (lastChar == 'd') {
                now->tm_mday += number;
            } else if (lastChar == 'm') {
                now->tm_mon += number;
            } else if (lastChar == 'y') {
                now->tm_year += number;
            }

            mktime(now); // Normalize the date
            returnDate = std::to_string(now->tm_year + 1900) + "-" + std::to_string(now->tm_mon + 1) + "-" + std::to_string(now->tm_mday);
        }
    }

    // if date format is "xx"
    if (returnDate.empty() && stringDate.length() <= 2 && stoi(stringDate) <= 31)
    {
        std::tm* now = std::localtime(&t);
        int year = now->tm_year + 1900; // tm_year is years since 1900
        int month;
        if (stoi(stringDate) < now->tm_mday) {
            month = now->tm_mon += 2; // tm_mon is months since January
        } else {
            month = now->tm_mon += 1; // tm_mon is months since January
        }
        returnDate = std::to_string(year) + "-" + std::to_string(month) + "-" + stringDate;
    }

    // if date format is "xxxx-xx-xx"
    if (returnDate.empty() && stringDate.find('-') != std::string::npos)
    {
        returnDate = stringDate;
    }

    std::tm tm = {};
    std::istringstream ss(returnDate);
    ss >> std::get_time(&tm, "%Y-%m-%d");
    if (ss.fail()) {
        throw std::invalid_argument("Invalid date format.");
    }

    return endOfDayTimestamp(std::mktime(&tm));
}

time_t DateHelpers::endOfDayTimestamp(time_t inputTimestamp) {
    // Convert to a tm
    std::tm* input_tm = std::localtime(&inputTimestamp);

    // Set the time to 23:59:59
    input_tm->tm_hour = 23;
    input_tm->tm_min = 59;
    input_tm->tm_sec = 59;

    // Convert back to a time_t
    time_t endOfDayTimestamp = std::mktime(input_tm);

    return endOfDayTimestamp;
}

bool DateHelpers::isDateValidFromUser(std::string date)
{
    // replace dots with dashes
    std::replace(date.begin(), date.end(), '.', '-');

    // if date is xxxx-xx-xx format
    std::regex dateRegex(R"(^(\d{4})-(\d{2})-(\d{2})$)");
    if (std::regex_match(date, dateRegex))
    {
        return true;
    }

    // if date is xx-xx format
    std::regex dateRegex2(R"(^(\d{2})-(\d{2})$)");
    if (std::regex_match(date, dateRegex2))
    {
        return true;
    }

    // if date is xx format
    std::regex dateRegex3(R"(^(\d{2})$)");
    if (std::regex_match(date, dateRegex3))
    {
        return true;
    }

    return false;
}

time_t DateHelpers::getTodayStart(int offset)
{
    time_t today_start;
    // get today at 00:00
    time(&today_start);
    tm* timeinfo = localtime(&today_start);
    timeinfo->tm_hour = 0;
    timeinfo->tm_min = 0;
    timeinfo->tm_sec = 0;
    // add offset
    timeinfo->tm_mday += offset;
    today_start = mktime(timeinfo);
    return today_start;
}

time_t DateHelpers::getTodayEnd(int offset)
{
    time_t start = getTodayStart(offset);
    time_t today_end = start + 24 * 60 * 60;
    return today_end;
}

time_t DateHelpers::getWeekStart(int offset)
{
    time_t week_start;
    // get Monday at 00:00
    time(&week_start);
    tm* timeinfo = localtime(&week_start);
    timeinfo->tm_hour = 0;
    timeinfo->tm_min = 0;
    timeinfo->tm_sec = 0;
    // get the current day of the week
    int current_day = timeinfo->tm_wday;
    // get the number of days to subtract to get to Monday
    int days_to_subtract = current_day - 1;
    if (current_day == 0) {
        days_to_subtract = 6;
    }
    // subtract the days
    timeinfo->tm_mday -= days_to_subtract;
    // add offset
    timeinfo->tm_mday += offset * 7;
    week_start = mktime(timeinfo);
    return week_start;
}

time_t DateHelpers::getWeekEnd(int offset)
{
    time_t start = getWeekStart(offset);
    time_t week_end = start + 7 * 24 * 60 * 60;
    return week_end;
}

time_t DateHelpers::getMonthStart(int offset)
{
    time_t month_start;
    // get the first day of the month at 00:00
    time(&month_start);
    tm* timeinfo = localtime(&month_start);
    timeinfo->tm_hour = 0;
    timeinfo->tm_min = 0;
    timeinfo->tm_sec = 0;
    timeinfo->tm_mday = 1;
    // add offset
    timeinfo->tm_mon += offset;
    month_start = mktime(timeinfo);
    return month_start;
}

time_t DateHelpers::getMonthEnd(int offset)
{
    time_t start = getMonthStart(offset);
    tm* timeinfo = localtime(&start);
    timeinfo->tm_mon += 1;
    time_t month_end = mktime(timeinfo);
    return month_end;
}

time_t DateHelpers::getYearStart(int offset)
{
    time_t year_start;
    // get the first day of the year at 00:00
    time(&year_start);
    tm* timeinfo = localtime(&year_start);
    timeinfo->tm_hour = 0;
    timeinfo->tm_min = 0;
    timeinfo->tm_sec = 0;
    timeinfo->tm_mday = 1;
    timeinfo->tm_mon = 0;
    // add offset
    timeinfo->tm_year += offset;
    year_start = mktime(timeinfo);
    return year_start;
}

time_t DateHelpers::getYearEnd(int offset)
{
    time_t start = getYearStart(offset);
    tm* timeinfo = localtime(&start);
    timeinfo->tm_year += 1;
    time_t year_end = mktime(timeinfo);
    return year_end;
}

time_t DateHelpers::getDayStart(const std::string& day, time_t timestamp)
{
    std::map<std::string, int> weekDays = {
        {"sunday", 0}, {"monday", 1}, {"tuesday", 2}, {"wednesday", 3},
        {"thursday", 4}, {"friday", 5}, {"saturday", 6}
    };

    if (weekDays.count(StringHelpers::toLower(day)) == 0) {
        throw std::invalid_argument("Invalid day name.");
    }

    tm* timeinfo = localtime(&timestamp);
    int current_day = timeinfo->tm_wday;
    int target_day = weekDays[StringHelpers::toLower(day)];
    int days_before_target = (current_day - target_day + 7) % 7;
    timeinfo->tm_mday -= days_before_target;
    timeinfo->tm_hour = 0;
    timeinfo->tm_min = 0;
    timeinfo->tm_sec = 0;
    mktime(timeinfo); // Normalize the date
    time_t r = mktime(timeinfo);
    return r;
}

time_t DateHelpers::getDayEnd(const std::string& day, time_t timestamp)
{
    time_t start = getDayStart(day, timestamp);
    time_t day_end = start + 24 * 60 * 60;
    return day_end;
}
