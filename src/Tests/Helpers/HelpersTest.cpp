#include "../../Helpers/DateHelpers.h"
#include "../../List/ListItems/PriorityService.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("isNameValid")
{
    PriorityService priorityService;
    SECTION("Valid priorities")
    {
        REQUIRE(priorityService.isNameValid("low"));
        REQUIRE(priorityService.isNameValid("medium"));
        REQUIRE(priorityService.isNameValid("high"));
        REQUIRE(priorityService.isNameValid("urgent"));
        REQUIRE(priorityService.isNameValid("critical"));
    }

    SECTION("Invalid priorities")
    {
        REQUIRE_FALSE(priorityService.isNameValid("invalid_priority"));
    }
}

TEST_CASE("isIdValid")
{
    PriorityService priorityService;
    SECTION("Valid priorities")
    {
        REQUIRE(priorityService.isIdValid(0));
        REQUIRE(priorityService.isIdValid(1));
        REQUIRE(priorityService.isIdValid(2));
        REQUIRE(priorityService.isIdValid(3));
        REQUIRE(priorityService.isIdValid(4));
    }

    SECTION("Invalid priorities")
    {
        REQUIRE_FALSE(priorityService.isIdValid(311));
    }
}

TEST_CASE("getIdFromName")
{
    PriorityService priorityService;
    SECTION("Valid priorities")
    {
        REQUIRE(priorityService.getIdFromName("low") == 0);
        REQUIRE(priorityService.getIdFromName("medium") == 1);
        REQUIRE(priorityService.getIdFromName("high") == 2);
        REQUIRE(priorityService.getIdFromName("urgent") == 3);
        REQUIRE(priorityService.getIdFromName("critical") == 4);
    }

    SECTION("Invalid priorities")
    {
        REQUIRE_THROWS_AS(priorityService.getIdFromName("invalid_priority"), std::invalid_argument);
    }
}

TEST_CASE("getNameFromId")
{
    PriorityService priorityService;
    SECTION("Valid priorities")
    {
        REQUIRE(priorityService.getNameFromId(0) == "low");
        REQUIRE(priorityService.getNameFromId(1) == "medium");
        REQUIRE(priorityService.getNameFromId(2) == "high");
        REQUIRE(priorityService.getNameFromId(3) == "urgent");
        REQUIRE(priorityService.getNameFromId(4) == "critical");
    }

    SECTION("Invalid priorities")
    {
        REQUIRE_THROWS_AS(priorityService.getNameFromId(5), std::invalid_argument);
    }
}

TEST_CASE("countCharsWithoutBashCodes")
{
    SECTION("Normal string")
    {
        REQUIRE(StringHelpers::countCharsWithoutBashCodes("test") == 4);
        REQUIRE(StringHelpers::countCharsWithoutBashCodes("test with spaces") == 16);
        REQUIRE(StringHelpers::countCharsWithoutBashCodes("test: with special char!") == 24);
        REQUIRE(StringHelpers::countCharsWithoutBashCodes("test: with backslash  !") == 23);
        REQUIRE(StringHelpers::countCharsWithoutBashCodes("test: with backslash \\!") == 23);
        REQUIRE(StringHelpers::countCharsWithoutBashCodes("OPS-2062 VP Société Général UK") == 30);
    }

    SECTION("String with styles")
    {
        REQUIRE(StringHelpers::countCharsWithoutBashCodes("\033[3mtest\033[0m") == 4);
        REQUIRE(StringHelpers::countCharsWithoutBashCodes("\033[3mtest with spaces\033[0m") == 16);
        REQUIRE(StringHelpers::countCharsWithoutBashCodes("\033[3mtest with ,.!\033[0m") == 13);
        REQUIRE(StringHelpers::countCharsWithoutBashCodes("\033[3mtest with \012[m,.!\033[0m") ==
                16); // code for \n[m (chosen randomly)
    }

    SECTION("Invalid priorities")
    {
        PriorityService priorityService;
        REQUIRE_THROWS_AS(priorityService.getNameFromId(5), std::invalid_argument);
    }
}

TEST_CASE("adjustStringLength")
{
    SECTION("Normal string to expand")
    {
        REQUIRE(StringHelpers::adjustStringLength("test", 20) == "test                ");
        REQUIRE(StringHelpers::adjustStringLength("test with spaces", 20) == "test with spaces    ");
        REQUIRE(StringHelpers::adjustStringLength("test: with special char!", 25) == "test: with special char! ");
        REQUIRE(StringHelpers::adjustStringLength("test: with backslash  !", 25) == "test: with backslash  !  ");
        REQUIRE(StringHelpers::adjustStringLength("test: with backslash \\!", 25) == "test: with backslash \\!  ");
        REQUIRE(StringHelpers::adjustStringLength("\033[3mtest\033[0m", 20) == "\033[3mtest\033[0m                ");
        REQUIRE(StringHelpers::adjustStringLength("\033[3mtest with spaces\033[0m", 20) ==
                "\033[3mtest with spaces\033[0m    ");
        REQUIRE(StringHelpers::adjustStringLength("\033[3mtest with ,.!\033[0m", 20) ==
                "\033[3mtest with ,.!\033[0m       ");
        REQUIRE(StringHelpers::adjustStringLength("OPS-2062 VP Société Général UK", 56) ==
                "OPS-2062 VP Société Général UK                          ");
    }

    SECTION("Normal string to shorten")
    {
        REQUIRE(StringHelpers::adjustStringLength("test", 2) == "t ");
        REQUIRE(StringHelpers::adjustStringLength("test", 4) == "test");
        REQUIRE(StringHelpers::adjustStringLength("test", 5) == "test ");
        REQUIRE(StringHelpers::adjustStringLength("testNumeroDeux", 10) == "testNum-  ");
        REQUIRE(StringHelpers::adjustStringLength("test with spaces", 10) == "test wi-  ");
        REQUIRE(StringHelpers::adjustStringLength("test: with special char!", 10) == "test: w-  ");
        REQUIRE(StringHelpers::adjustStringLength("test: with backslash  !", 10) == "test: w-  ");
        REQUIRE(StringHelpers::adjustStringLength("test: with backslash \\!", 20) == "test: with backsl-  ");
        REQUIRE(StringHelpers::adjustStringLength("\033[3mtest\033[0m", 2) == "t ");
        REQUIRE(StringHelpers::adjustStringLength("\033[3mtest with spaces\033[0m", 10) == "test wi-  ");
        REQUIRE(StringHelpers::adjustStringLength("\033[3mtest with ,.!\033[0m", 4) == "tes ");
    }

    SECTION("Invalid priorities")
    {
        PriorityService priorityService;
        REQUIRE_THROWS_AS(priorityService.getNameFromId(5), std::invalid_argument);
    }
}

TEST_CASE("containsWord", "[Helpers]")
{
    SECTION("Word is present in the string")
    {
        std::string str = "Hello, this is a test string";
        std::string word = "test";
        REQUIRE(StringHelpers::containsWord(str, word) == true);
    }

    SECTION("Word is not present in the string")
    {
        std::string str = "Hello, this is a test string";
        std::string word = "absent";
        REQUIRE(StringHelpers::containsWord(str, word) == false);
    }

    SECTION("Empty string")
    {
        std::string str;
        std::string word = "test";
        REQUIRE(StringHelpers::containsWord(str, word) == false);
    }

    SECTION("Empty word")
    {
        std::string str = "Hello, this is a test string";
        std::string word;
        REQUIRE(StringHelpers::containsWord(str, word) == false);
    }

    SECTION("half word")
    {
        std::string str = "Hello, this is a test string";
        std::string word = "Hel";
        REQUIRE(StringHelpers::containsWord(str, word) == false);
    }

    SECTION("uppercase/lowercase")
    {
        std::string str = "Hello, this is a test string";
        std::string word = "hello";
        REQUIRE(StringHelpers::containsWord(str, word) == false);
    }

    SECTION("with string and number")
    {
        std::string str = "Hello, this is a test2string";
        std::string word = "test";
        REQUIRE(StringHelpers::containsWord(str, word) == false);
    }
}

TEST_CASE("Helpers::timestampToDuration function", "[Helpers]")
{
    SECTION("Test duration for 1 year, 0 month, 1 day, 1 hour, 1 minute and 1 second")
    {
        time_t start = 0;
        time_t end = 24 * 60 * 60 * 366 + 24 * 60 * 60 + 60 * 60 + 60 + 1;
        REQUIRE(DateHelpers::timestampToDuration(start, end) == "> 1y");
    }
    SECTION("Test duration for 1 year, 1 month, 1 day, 1 hour, 1 minute and 1 second")
    {
        time_t start = 0;
        time_t end = 24 * 60 * 60 * 366 + 24 * 60 * 60 * 31 + 24 * 60 * 60 + 60 * 60 + 60 + 1;
        REQUIRE(DateHelpers::timestampToDuration(start, end) == "> 1y");
    }

    SECTION("Test duration for 10 days, 1 hour, 1 minute and 1 second")
    {
        time_t start = 0;
        time_t end = 24 * 60 * 60 * 10 + 60 * 60 + 60 + 1; // 10 day, 1 hour, 1 minute and 1 second
        REQUIRE(DateHelpers::timestampToDuration(start, end) == "10d 1h 1m 1s");
    }

    SECTION("Test duration for 110 days")
    {
        time_t start = 0;
        time_t end = 24 * 60 * 60 * 110;
        REQUIRE(DateHelpers::timestampToDuration(start, end) == "110d");
    }

    SECTION("Test duration for 110 days, 1 hour, 1 minute and 1 second")
    {
        time_t start = 0;
        time_t end = 24 * 60 * 60 * 110 + 60 * 60 + 60 + 1;
        REQUIRE(DateHelpers::timestampToDuration(start, end) == "110d 1h");
    }

    SECTION("Test duration for 1 day, 1 hour, 1 minute and 1 second")
    {
        time_t start = 0;
        time_t end = 24 * 60 * 60 + 60 * 60 + 60 + 1; // 1 day, 1 hour, 1 minute and 1 second
        REQUIRE(DateHelpers::timestampToDuration(start, end) == "1d 1h 1m 1s");
    }

    SECTION("Test duration for 0 day, 0 hour, 0 minute and 0 second")
    {
        time_t start = 0;
        time_t end = 0; // 0 day, 0 hour, 0 minute and 0 second
        REQUIRE(DateHelpers::timestampToDuration(start, end) == "0s");
    }

    SECTION("Test duration for 2 days, 3 hours, 4 minutes and 5 seconds")
    {
        time_t start = 0;
        time_t end = 2 * 24 * 60 * 60 + 3 * 60 * 60 + 4 * 60 + 5; // 2 days, 3 hours, 4 minutes and 5 seconds
        REQUIRE(DateHelpers::timestampToDuration(start, end) == "2d 3h 4m 5s");
    }
}

TEST_CASE("Helpers::formatTimestamp function", "[Helpers]")
{
    SECTION("Test timestamp formatting")
    {
        time_t timestamp = 946684800; // Represents 01/01/2000 - 01:00
        REQUIRE(DateHelpers::formatTimestamp(timestamp) == "01.01.2000 - 01:00");
    }

    SECTION("Test timestamp formatting")
    {
        time_t timestamp = 978307200; // Represents 01/01/2001 - 01:00
        REQUIRE(DateHelpers::formatTimestamp(timestamp) == "01.01.2001 - 01:00");
    }

    SECTION("Test timestamp formatting")
    {
        time_t timestamp = 1009843200; // Represents 01/01/2002 - 01:00
        REQUIRE(DateHelpers::formatTimestamp(timestamp) == "01.01.2002 - 01:00");
    }

    SECTION("Test timestamp formatting")
    {
        time_t timestamp = 1009843200; // Represents 01/01/2002 - 01:00
        REQUIRE(DateHelpers::formatTimestamp(timestamp, "default") == "01.01.2002 - 01:00");
    }

    SECTION("Test timestamp formatting")
    {
        time_t timestamp = 1009843200; // Represents 01/01/2002 - 01:00
        REQUIRE(DateHelpers::formatTimestamp(timestamp, "long") == "01.01.2002 - 01:00:00");
    }

    SECTION("Test timestamp formatting")
    {
        time_t timestamp = 1009843200; // Represents 01/01/2002 - 01:00
        REQUIRE(DateHelpers::formatTimestamp(timestamp, "date") == "01.01.2002");
    }

    SECTION("Test timestamp formatting")
    {
        time_t timestamp = 1009843200; // Represents 01/01/2002 - 01:00
        REQUIRE(DateHelpers::formatTimestamp(timestamp, "time") == "01:00:00");
    }
}

TEST_CASE("StringHelper::randomString function", "[Helpers]")
{
    SECTION("Test random string length")
    {
        int len = 10;
        std::string result = StringHelpers::randomString(len);
        REQUIRE(result.length() == len);
    }

    SECTION("Test random string uniqueness")
    {
        int len = 20;
        std::string result1 = StringHelpers::randomString(len);
        std::string result2 = StringHelpers::randomString(len);
        REQUIRE(result1 != result2);
    }

    SECTION("Test string length")
    {
        int len = 20;
        std::string result1 = StringHelpers::randomString(len);
        std::string result2 = StringHelpers::randomString(len);
        REQUIRE(result1.length() == len);
        REQUIRE(result2.length() == len);
    }
}

TEST_CASE("Helpers::toLower function", "[Helpers]")
{
    SECTION("Test toLower method")
    {
        // Call the toLower method
        std::string test1 = StringHelpers::toLower("HELLO");
        REQUIRE(test1 == "hello");
        std::string test2 = StringHelpers::toLower("HEllO");
        REQUIRE(test2 == "hello");
        std::string test3 = StringHelpers::toLower("HEllO, bonSOIR.");
        REQUIRE(test3 == "hello, bonsoir.");
        std::string test4 = StringHelpers::toLower("123445");
        REQUIRE(test4 == "123445");

        // Add REQUIRE statements to check the expected behavior
    }
}

TEST_CASE("Helpers::filterAlnumAndSpace function", "[Helpers]")
{
    SECTION("Test filterAlnumAndSpace method")
    {
        std::string test1 = StringHelpers::filterAlnumAndSpace("HELLO 123");
        REQUIRE(test1 == "HELLO 123");
        std::string test2 = StringHelpers::filterAlnumAndSpace("HELLO, 123");
        REQUIRE(test2 == "HELLO 123");
        std::string test3 = StringHelpers::filterAlnumAndSpace("HELLO, 123!--");
        REQUIRE(test3 == "HELLO 123");
    }
}

TEST_CASE("Helpers::isBeginningOfWord function", "[Helpers]")
{
    SECTION("Test isBeginningOfWord method")
    {
        bool test1 = StringHelpers::isBeginningOfWord("Hello", "He");
        REQUIRE(test1 == true);
        bool test2 = StringHelpers::isBeginningOfWord("bonjour", "bon");
        REQUIRE(test2 == true);
        bool test3 = StringHelpers::isBeginningOfWord("bonjour tout le monde", "bon");
        REQUIRE(test3 == true);
        bool test4 = StringHelpers::isBeginningOfWord("bonjour tout le monde", "tout");
        REQUIRE(test4 == false);
    }
}

TEST_CASE("Helpers::isBeginningOfAnyWords function", "[Helpers]")
{
    SECTION("Test isBeginningOfAnyWords method")
    {
        bool test1 = StringHelpers::isBeginningOfAnyWords("Hello", "He");
        REQUIRE(test1 == true);
        bool test2 = StringHelpers::isBeginningOfAnyWords("bonjour", "bon");
        REQUIRE(test2 == true);
        bool test3 = StringHelpers::isBeginningOfAnyWords("bonjour tout le monde", "bon");
        REQUIRE(test3 == true);
        bool test4 = StringHelpers::isBeginningOfAnyWords("bonjour tout le monde", "to");
        REQUIRE(test4 == true);
        bool test5 = StringHelpers::isBeginningOfAnyWords("bonjour tout le monde", "le");
        REQUIRE(test5 == true);
    }
}

TEST_CASE("escapeChar", "[StringHelpers]")
{
    StringHelpers stringHelpers;
    std::string test1 = "bonjour tout le monde";
    std::string expectedTest1 = "bonjour\\ tout\\ le\\ monde";
    std::string resultTest1 = StringHelpers::escapeChar(test1, ' ');
    REQUIRE(resultTest1 == expectedTest1);

    std::string test2 = "bonjour, tout le monde! Ça va ?";
    std::string expectedTest2 = "bonjour,\\ tout\\ le\\ monde!\\ Ça\\ va\\ ?";
    std::string resultTest2 = StringHelpers::escapeChar(test2, ' ');
    REQUIRE(resultTest2 == expectedTest2);
}

TEST_CASE("StringHelper::randomAlNumString function", "[Helpers]")
{
    SECTION("Test random alphanumeric string length")
    {
        int len = 10;
        std::string result = StringHelpers::randomAlNumString(len);
        REQUIRE(result.length() == len);
    }

    SECTION("Test random alphanumeric string contains only alnum chars")
    {
        std::string result = StringHelpers::randomAlNumString(20);
        REQUIRE(StringHelpers::isAlnum(result));
    }
}

TEST_CASE("StringHelper::randomLettersLowercase function", "[Helpers]")
{
    SECTION("Test random lowercase string length")
    {
        int len = 10;
        std::string result = StringHelpers::randomLettersLowercase(len);
        REQUIRE(result.length() == len);
    }

    SECTION("Test random lowercase string is all lowercase")
    {
        std::string result = StringHelpers::randomLettersLowercase(20);
        REQUIRE(result == StringHelpers::toLower(result));
    }
}

TEST_CASE("StringHelper::toUpper function", "[Helpers]")
{
    REQUIRE(StringHelpers::toUpper("hello") == "HELLO");
    REQUIRE(StringHelpers::toUpper("HeLLo") == "HELLO");
    REQUIRE(StringHelpers::toUpper("123abc") == "123ABC");
    REQUIRE(StringHelpers::toUpper("") == "");
}

TEST_CASE("StringHelper::containsString function", "[Helpers]")
{
    REQUIRE(StringHelpers::containsString("hello world", "world") == true);
    REQUIRE(StringHelpers::containsString("hello world", "llo") == true);
    REQUIRE(StringHelpers::containsString("hello world", "xyz") == false);
    REQUIRE(StringHelpers::containsString("hello", "") == true);
    REQUIRE(StringHelpers::containsString("", "hello") == false);
}

TEST_CASE("StringHelper::split function", "[Helpers]")
{
    SECTION("Split by space")
    {
        std::vector<std::string> result = StringHelpers::split("hello world foo", ' ');
        REQUIRE(result.size() == 3);
        REQUIRE(result[0] == "hello");
        REQUIRE(result[1] == "world");
        REQUIRE(result[2] == "foo");
    }

    SECTION("Split by comma")
    {
        std::vector<std::string> result = StringHelpers::split("a,b,c", ',');
        REQUIRE(result.size() == 3);
        REQUIRE(result[0] == "a");
        REQUIRE(result[1] == "b");
        REQUIRE(result[2] == "c");
    }

    SECTION("Split single item")
    {
        std::vector<std::string> result = StringHelpers::split("hello", ' ');
        REQUIRE(result.size() == 1);
        REQUIRE(result[0] == "hello");
    }

    SECTION("Split empty string")
    {
        std::vector<std::string> result = StringHelpers::split("", ' ');
        REQUIRE(result.empty());
    }
}

TEST_CASE("StringHelper::isAlnum function", "[Helpers]")
{
    REQUIRE(StringHelpers::isAlnum("hello123") == true);
    REQUIRE(StringHelpers::isAlnum("HELLO") == true);
    REQUIRE(StringHelpers::isAlnum("123") == true);
    REQUIRE(StringHelpers::isAlnum("hello world") == false);
    REQUIRE(StringHelpers::isAlnum("hello!") == false);
    REQUIRE(StringHelpers::isAlnum("") == true);
}

TEST_CASE("StringHelper::colorize function", "[Helpers]")
{
    std::string result = StringHelpers::colorize("hello", "\033[31m");
    REQUIRE(result == "\033[31mhello\033[0m");

    std::string result2 = StringHelpers::colorize("", "\033[32m");
    REQUIRE(result2 == "\033[32m\033[0m");
}

TEST_CASE("StringHelper::adjustStringLengthWithString function", "[Helpers]")
{
    SECTION("Pad with custom string")
    {
        std::string result = StringHelpers::adjustStringLengthWithString("hi", 6, "-");
        REQUIRE(result == "hi----");
    }

    SECTION("String already long enough returns as-is")
    {
        std::string result = StringHelpers::adjustStringLengthWithString("hello", 3, "-");
        REQUIRE(result == "hello");
    }

    SECTION("Exact length returns as-is")
    {
        std::string result = StringHelpers::adjustStringLengthWithString("hello", 5, "-");
        REQUIRE(result == "hello");
    }
}

TEST_CASE("StringHelper::stringToWstring and wstringToString round-trip", "[Helpers]")
{
    std::string original = "Hello World";
    std::wstring wide = StringHelpers::stringToWstring(original);
    std::string back = StringHelpers::wstringToString(wide);
    REQUIRE(back == original);
}

TEST_CASE("DateHelper::endOfDayTimestamp function", "[DateHelpers]")
{
    time_t now = time(nullptr);
    time_t endOfDay = DateHelpers::endOfDayTimestamp(now);
    tm* timeinfo = localtime(&endOfDay);

    REQUIRE(timeinfo->tm_hour == 23);
    REQUIRE(timeinfo->tm_min == 59);
    REQUIRE(timeinfo->tm_sec == 59);
}

TEST_CASE("DateHelper::isTimestampToday function", "[DateHelpers]")
{
    time_t now = time(nullptr);
    REQUIRE(DateHelpers::isTimestampToday(now) == true);

    // A very old timestamp should not be today
    time_t oldTimestamp = 1000000;
    REQUIRE(DateHelpers::isTimestampToday(oldTimestamp) == false);
}

TEST_CASE("DateHelper::isTimestampNDaysFromToday function", "[DateHelpers]")
{
    time_t now = time(nullptr);
    REQUIRE(DateHelpers::isTimestampNDaysFromToday(now, 0) == true);

    // Tomorrow's timestamp should be 1 day from today
    time_t tomorrow = now + 24 * 60 * 60;
    REQUIRE(DateHelpers::isTimestampNDaysFromToday(tomorrow, 1) == true);
    REQUIRE(DateHelpers::isTimestampNDaysFromToday(tomorrow, 0) == false);
}

TEST_CASE("DateHelper::isDateValidFromUser function", "[DateHelpers]")
{
    REQUIRE(DateHelpers::isDateValidFromUser("2024-03-15") == true);
    REQUIRE(DateHelpers::isDateValidFromUser("2024.03.15") == true);
    REQUIRE(DateHelpers::isDateValidFromUser("03-15") == true);
    REQUIRE(DateHelpers::isDateValidFromUser("03.15") == true);
    REQUIRE(DateHelpers::isDateValidFromUser("15") == true);
    REQUIRE(DateHelpers::isDateValidFromUser("today") == false);
    REQUIRE(DateHelpers::isDateValidFromUser("abc") == false);
    REQUIRE(DateHelpers::isDateValidFromUser("1") == false);
}

TEST_CASE("DateHelper::formatTimestampToHumanDate function", "[DateHelpers]")
{
    SECTION("Returns N/A for zero timestamp")
    {
        REQUIRE(DateHelpers::formatTimestampToHumanDate(0) == "N/A");
    }

    SECTION("Returns formatted date for valid timestamp")
    {
        time_t timestamp = 1709506799; // 2024-03-03
        std::string result = DateHelpers::formatTimestampToHumanDate(timestamp, "default");
        REQUIRE(result.empty() == false);
    }

    SECTION("Returns date-only format")
    {
        time_t timestamp = 1709506799;
        std::string result = DateHelpers::formatTimestampToHumanDate(timestamp, "date");
        REQUIRE(result.empty() == false);
    }
}

TEST_CASE("DateHelper::getMonthStart and getMonthEnd", "[DateHelpers]")
{
    time_t monthStart = DateHelpers::getMonthStart(0);
    time_t monthEnd = DateHelpers::getMonthEnd(0);

    tm* startInfo = localtime(&monthStart);
    REQUIRE(startInfo->tm_mday == 1);
    REQUIRE(startInfo->tm_hour == 0);
    REQUIRE(startInfo->tm_min == 0);
    REQUIRE(startInfo->tm_sec == 0);

    REQUIRE(monthEnd > monthStart);

    // Next month start should equal this month end
    time_t nextMonthStart = DateHelpers::getMonthStart(1);
    REQUIRE(nextMonthStart == monthEnd);
}

TEST_CASE("DateHelper::getYearStart and getYearEnd", "[DateHelpers]")
{
    time_t yearStart = DateHelpers::getYearStart(0);
    tm startInfo = *localtime(&yearStart);
    REQUIRE(startInfo.tm_mday == 1);
    REQUIRE(startInfo.tm_mon == 0); // January
    REQUIRE(startInfo.tm_hour == 0);

    time_t yearEnd = DateHelpers::getYearEnd(0);
    REQUIRE(yearEnd > yearStart);

    // Next year start should equal this year end
    time_t nextYearStart = DateHelpers::getYearStart(1);
    REQUIRE(nextYearStart == yearEnd);
}

TEST_CASE("DateHelper::getDayEnd function", "[DateHelpers]")
{
    time_t timestamp = 1717144878;
    time_t dayStart = DateHelpers::getDayStart("monday", timestamp);
    time_t dayEnd = DateHelpers::getDayEnd("monday", timestamp);

    REQUIRE(dayEnd == dayStart + 24 * 60 * 60);
}

TEST_CASE("DateHelper::relativeDateToTimestamp special values", "[DateHelpers]")
{
    SECTION("empty string returns 0")
    {
        REQUIRE(DateHelpers::relativeDateToTimestamp("") == 0);
    }

    SECTION("reset/clear/none return 0")
    {
        REQUIRE(DateHelpers::relativeDateToTimestamp("reset") == 0);
        REQUIRE(DateHelpers::relativeDateToTimestamp("clear") == 0);
        REQUIRE(DateHelpers::relativeDateToTimestamp("none") == 0);
    }

    SECTION("now returns current timestamp")
    {
        time_t before = time(nullptr);
        time_t result = DateHelpers::relativeDateToTimestamp("now");
        time_t after = time(nullptr);
        REQUIRE(result >= before);
        REQUIRE(result <= after);
    }

    SECTION("end-of-day returns today at 23:59:59")
    {
        time_t result = DateHelpers::relativeDateToTimestamp("end-of-day");
        tm* timeinfo = localtime(&result);
        REQUIRE(timeinfo->tm_hour == 23);
        REQUIRE(timeinfo->tm_min == 59);
        REQUIRE(timeinfo->tm_sec == 59);
    }

    SECTION("weekday names work")
    {
        time_t referenceTime = time(nullptr);
        REQUIRE_NOTHROW(DateHelpers::relativeDateToTimestamp("monday", referenceTime));
        REQUIRE_NOTHROW(DateHelpers::relativeDateToTimestamp("friday", referenceTime));
        REQUIRE_NOTHROW(DateHelpers::relativeDateToTimestamp("sunday", referenceTime));
    }
}

TEST_CASE("DateHelper::relativeDateToTimestamp function", "[Helpers]")
{
    SECTION("Test relativeDateToTimestamp method")
    {
        time_t referenceTime = time(nullptr);

        std::string timeTest1 = "2024-03-03";
        time_t test1 = DateHelpers::relativeDateToTimestamp(timeTest1);
        REQUIRE(test1 == 1709506799);

        std::string timeTest2 = "2024.03.03";
        time_t test2 = DateHelpers::relativeDateToTimestamp(timeTest2);
        REQUIRE(test2 == 1709506799);

        std::string timeTest3 = "03.03";
        REQUIRE_NOTHROW(DateHelpers::relativeDateToTimestamp(timeTest3));

        std::string timeTest4 = "03";
        REQUIRE_NOTHROW(DateHelpers::relativeDateToTimestamp(timeTest4));

        std::string timeTest5 = "3";
        REQUIRE_NOTHROW(DateHelpers::relativeDateToTimestamp(timeTest5));

        std::string timeTest6 = "12432423";
        REQUIRE_THROWS(DateHelpers::relativeDateToTimestamp(timeTest6));

        std::string timeTest7 = "today";
        time_t test7 = DateHelpers::relativeDateToTimestamp(timeTest7, referenceTime);
        REQUIRE(DateHelpers::formatTimestamp(test7, "ymd") == DateHelpers::formatTimestamp(referenceTime, "ymd"));

        std::string timeTest8 = "tomorrow";
        time_t test8 = DateHelpers::relativeDateToTimestamp(timeTest8, referenceTime);
        REQUIRE(DateHelpers::formatTimestamp(test8, "ymd") ==
                DateHelpers::formatTimestamp(referenceTime + 24 * 60 * 60, "ymd"));

        std::string timeTest9 = "next-week";
        time_t test9 = DateHelpers::relativeDateToTimestamp(timeTest9, referenceTime);
        REQUIRE(DateHelpers::formatTimestamp(test9, "ymd") ==
                DateHelpers::formatTimestamp(referenceTime + 7 * 24 * 60 * 60, "ymd"));

        std::string timeTest10 = "1d";
        time_t test10 = DateHelpers::relativeDateToTimestamp(timeTest10, referenceTime);
        REQUIRE(DateHelpers::formatTimestamp(test10, "ymd") ==
                DateHelpers::formatTimestamp(referenceTime + 24 * 60 * 60, "ymd"));

        std::string timeTest11 = "1day";
        time_t test11 = DateHelpers::relativeDateToTimestamp(timeTest11, referenceTime);
        REQUIRE(DateHelpers::formatTimestamp(test11, "ymd") ==
                DateHelpers::formatTimestamp(referenceTime + 24 * 60 * 60, "ymd"));

        std::string timeTest12 = "2days";
        time_t test12 = DateHelpers::relativeDateToTimestamp(timeTest12, referenceTime);
        REQUIRE(DateHelpers::formatTimestamp(test12, "ymd") ==
                DateHelpers::formatTimestamp(referenceTime + 2 * 24 * 60 * 60, "ymd"));

        std::string timeTest13 = "31days";
        time_t test13 = DateHelpers::relativeDateToTimestamp(timeTest13, referenceTime);
        REQUIRE(DateHelpers::formatTimestamp(test13, "ymd") ==
                DateHelpers::formatTimestamp(referenceTime + 31 * 24 * 60 * 60, "ymd"));

        // >= instead of == because if we are the 31, we are two months later
        std::string timeTest14 = "1month";
        time_t test14 = DateHelpers::relativeDateToTimestamp(timeTest14, referenceTime);
        REQUIRE(stoi(DateHelpers::formatTimestamp(test14, "m")) >=
                stoi(DateHelpers::formatTimestamp(referenceTime, "m")) + 1);

        // >= instead of == because if we are the 31, we are two months later
        std::string timeTest15 = "1m";
        time_t test15 = DateHelpers::relativeDateToTimestamp(timeTest15, referenceTime);
        REQUIRE(stoi(DateHelpers::formatTimestamp(test15, "m")) >=
                stoi(DateHelpers::formatTimestamp(referenceTime, "m")) + 1);

        // >= instead of == because if we are the 31, we are three months later
        std::string timeTest16 = "2m";
        time_t test16 = DateHelpers::relativeDateToTimestamp(timeTest16, referenceTime);
        REQUIRE(stoi(DateHelpers::formatTimestamp(test16, "m")) >=
                stoi(DateHelpers::formatTimestamp(referenceTime, "m")) + 2);

        std::string timeTest17 = "2months";
        time_t test17 = DateHelpers::relativeDateToTimestamp(timeTest17, referenceTime);
        REQUIRE(stoi(DateHelpers::formatTimestamp(test17, "m")) >=
                stoi(DateHelpers::formatTimestamp(referenceTime, "m")) + 2);

        std::string timeTest18 = "1y";
        time_t test18 = DateHelpers::relativeDateToTimestamp(timeTest18, referenceTime);
        REQUIRE(stoi(DateHelpers::formatTimestamp(test18, "y")) ==
                stoi(DateHelpers::formatTimestamp(referenceTime, "y")) + 1);

        std::string timeTest19 = "1year";
        time_t test19 = DateHelpers::relativeDateToTimestamp(timeTest19, referenceTime);
        REQUIRE(stoi(DateHelpers::formatTimestamp(test19, "y")) ==
                stoi(DateHelpers::formatTimestamp(referenceTime, "y")) + 1);

        std::string timeTest20 = "2y";
        time_t test20 = DateHelpers::relativeDateToTimestamp(timeTest20, referenceTime);
        REQUIRE(stoi(DateHelpers::formatTimestamp(test20, "y")) ==
                stoi(DateHelpers::formatTimestamp(referenceTime, "y")) + 2);

        std::string timeTest21 = "2years";
        time_t test21 = DateHelpers::relativeDateToTimestamp(timeTest21, referenceTime);
        REQUIRE(stoi(DateHelpers::formatTimestamp(test21, "y")) ==
                stoi(DateHelpers::formatTimestamp(referenceTime, "y")) + 2);
    }
}

TEST_CASE("check the start - end calculation", "[DateHelpers]")
{
    time_t todayStart = DateHelpers::getTodayStart(0);
    time_t todayEnd = DateHelpers::getTodayEnd(0);

    REQUIRE(todayStart == todayEnd + 24 * 60 * 60 * -1);

    time_t yesterdayStart = DateHelpers::getTodayStart(-1);
    time_t yesterdayEnd = DateHelpers::getTodayEnd(-1);

    REQUIRE(yesterdayStart == yesterdayEnd + 24 * 60 * 60 * -1);

    time_t tomorrowStart = DateHelpers::getTodayStart(1);
    time_t tomorrowEnd = DateHelpers::getTodayEnd(1);

    REQUIRE(tomorrowStart == tomorrowEnd + 24 * 60 * 60 * -1);

    time_t thisWeekStart = DateHelpers::getWeekStart(0);
    time_t thisWeekEnd = DateHelpers::getWeekEnd(0);

    REQUIRE(thisWeekStart == thisWeekEnd + 7 * 24 * 60 * 60 * -1);

    time_t lastWeekStart = DateHelpers::getWeekStart(-1);
    time_t lastWeekEnd = DateHelpers::getWeekEnd(-1);

    REQUIRE(lastWeekStart == lastWeekEnd + 7 * 24 * 60 * 60 * -1);

    time_t nextWeekStart = DateHelpers::getWeekStart(1);
    time_t nextWeekEnd = DateHelpers::getWeekEnd(1);

    REQUIRE(nextWeekStart == nextWeekEnd + 7 * 24 * 60 * 60 * -1);
}

TEST_CASE("getTodayStart returns the start of today", "[DateHelpers]")
{
    time_t todayStart = DateHelpers::getTodayStart(0);
    tm* timeinfo = localtime(&todayStart);
    time_t now = time(nullptr);
    tm* nowinfo = localtime(&now);

    REQUIRE(timeinfo->tm_year == nowinfo->tm_year);
    REQUIRE(timeinfo->tm_mon == nowinfo->tm_mon);
    REQUIRE(timeinfo->tm_mday == nowinfo->tm_mday);
    //    REQUIRE(timeinfo->tm_hour == 0);
    //    REQUIRE(timeinfo->tm_min == 0);
    //    REQUIRE(timeinfo->tm_sec == 0);
}

TEST_CASE("getTodayEnd returns the end of today", "[DateHelpers]")
{
    time_t todayEnd = DateHelpers::getTodayEnd(0);
    tm* timeinfo = localtime(&todayEnd);
    time_t now = time(nullptr);
    tm* nowinfo = localtime(&now);

    REQUIRE(timeinfo->tm_year == nowinfo->tm_year);
    REQUIRE(timeinfo->tm_mon == nowinfo->tm_mon);
    REQUIRE(timeinfo->tm_mday == nowinfo->tm_mday);
    //    REQUIRE(timeinfo->tm_hour == 23);
    //    REQUIRE(timeinfo->tm_min == 59);
    //    REQUIRE(timeinfo->tm_sec == 59);
}

TEST_CASE("getDayStart returns correct timestamp for given day", "[DateHelpers]")
{
    DateHelpers dateHelpers;
    std::string day = "monday";
    time_t timestamp = 1717144878;
    time_t dayStart = DateHelpers::getDayStart(day, timestamp);
    tm* timeinfo = localtime(&dayStart);

    REQUIRE(timeinfo->tm_wday == 1); // Monday is represented as 1 in tm structure
    REQUIRE(timeinfo->tm_hour == 0);
    REQUIRE(timeinfo->tm_min == 0);
    REQUIRE(timeinfo->tm_sec == 0);

    REQUIRE(1716760800 == dayStart);
}

TEST_CASE("getDayStart throws exception for invalid day name", "[DateHelpers]")
{
    DateHelpers dateHelpers;
    std::string day = "invalid_day";
    time_t timestamp = time(nullptr);

    REQUIRE_THROWS_AS(dateHelpers.getDayStart(day, timestamp), std::invalid_argument);
}

TEST_CASE("getDayStart handles case sensitivity", "[DateHelpers]")
{
    DateHelpers dateHelpers;
    std::string day = "MoNdAy";
    time_t timestamp = time(nullptr);
    time_t dayStart = DateHelpers::getDayStart(day, timestamp);
    tm* timeinfo = localtime(&dayStart);

    REQUIRE(timeinfo->tm_wday == 1); // Monday is represented as 1 in tm structure
    REQUIRE(timeinfo->tm_hour == 0);
    REQUIRE(timeinfo->tm_min == 0);
    REQUIRE(timeinfo->tm_sec == 0);
}
