#ifndef TODOOS_STRINGHELPERS_H
#define TODOOS_STRINGHELPERS_H

#include <string>
#include <vector>
#include <codecvt>
#include <iomanip>
#include <unistd.h>
#include <sstream>
#include <algorithm>
#include <regex>
#include <chrono>
#include <iterator>

class StringHelpers {
public:
    static std::string randomString(int len = 4);
    static std::string randomAlNumString(int len = 4);
    static std::wstring stringToWstring(const std::string& str);
    static std::string wstringToString(const std::wstring& str);
    static std::string colorize(const std::string& text, const std::string& color);
    static std::string adjustStringLength(const std::string& str, int length, char fill = ' ');
    static std::string adjustStringLengthWithString(std::string str, int length, const std::string& fill = "");
    static std::size_t countCharsWithoutBashCodes(const std::string& str);
    static bool containsWord(const std::string& str, const std::string& word);
    static bool isBeginningOfWord(const std::string& str, const std::string& word);
    static bool isBeginningOfAnyWords(const std::string& completeWord, const std::string& wordToSearch);
    static bool containsString(const std::string& str, const std::string& word);
    static std::string toUpper(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string filterAlnumAndSpace(const std::string& str);
    static std::vector<std::string> split(const std::string& basicString, char i);
    static bool isAlnum(const std::string& str);
};


#endif //TODOOS_STRINGHELPERS_H
