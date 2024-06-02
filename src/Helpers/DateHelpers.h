#ifndef TODOOS_DATEHELPERS_H
#define TODOOS_DATEHELPERS_H

#include <string>
#include <vector>
#include <codecvt>
#include <iomanip>
#include <unistd.h>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <regex>
#include <chrono>
#include "StringHelpers.h"

class DateHelpers {
public:
    static std::string formatTimestamp(time_t timestamp, std::string format = "default", const std::string& separator = ".");
    static std::string formatTimestampToHumanDate(time_t timestamp, std::string format = "default");
    static bool isTimestampToday(time_t timestamp);
    static bool isTimestampNDaysFromToday(time_t timestamp, int n);
    static std::string timestampToDuration(time_t startedAt, time_t endedAt);
    static time_t relativeDateToTimestamp(std::string stringDate, time_t referenceTime = 0);
    static time_t endOfDayTimestamp(time_t timestamp);
    static bool isDateValidFromUser(std::string date);
    static time_t getTodayStart(int offset = 0);
    static time_t getTodayEnd(int offset = 0);
    static time_t getWeekStart(int offset = 0);
    static time_t getWeekEnd(int offset = 0);
    static time_t getMonthStart(int offset = 0);
    static time_t getMonthEnd(int offset = 0);
    static time_t getYearStart(int offset = 0);
    static time_t getYearEnd(int offset = 0);
    static time_t getDayStart(const std::string& day, time_t timestamp);
    static time_t getDayEnd(const std::string& day, time_t timestamp);
};


#endif //TODOOS_DATEHELPERS_H
