#include "StringHelpers.h"


std::string StringHelpers::randomString(const int len) {
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    seed ^= getpid() + std::clock();
    srand(seed);
    static const char alphanum[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    std::string tmp_s;
    tmp_s.reserve(len);

    for (int i = 0; i < len; ++i) {
        tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
    }

    return tmp_s;
}

std::string StringHelpers::randomAlNumString(const int len) {
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    seed ^= getpid() + std::clock();
    srand(seed);
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    std::string tmp_s;
    tmp_s.reserve(len);

    for (int i = 0; i < len; ++i) {
        tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
    }

    return tmp_s;
}

std::wstring StringHelpers::stringToWstring(const std::string& str) {
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.from_bytes(str);
}

std::string StringHelpers::wstringToString(const std::wstring& wstr) {
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.to_bytes(wstr);
}

std::string StringHelpers::colorize(const std::string& text, const std::string& color) {
    return color + text + "\033[0m";
}

std::string StringHelpers::adjustStringLength(const std::string& str, int length = 10, const char fill) {
    std::size_t inputLength = countCharsWithoutBashCodes(str);
    std::wstring adjustedwString = StringHelpers::stringToWstring(str);
    std::string adjustedString;

    // Pad with spaces if the string is shorter than length characters
    if (inputLength < length) {
        adjustedwString.append(length - inputLength, fill);
        return StringHelpers::wstringToString(adjustedwString);
    }

    // Truncate if the string is longer than length characters

    // Remove all bash codes from the string
    // shrink the string
    // TODO: implement a way to put back the bash codes
    std::regex ansi_escape_code("\033\[[0-9;]+m");
    std::wstring result = StringHelpers::stringToWstring(std::regex_replace(str, ansi_escape_code, ""));

    if (inputLength == length) {
        adjustedwString = result;
        adjustedString = StringHelpers::wstringToString(adjustedwString);
    }
    else if (inputLength > length && length > 5)
    {
        result = result.substr(0, length - 3);
        adjustedwString = result;
        adjustedString = StringHelpers::wstringToString(adjustedwString) + "-  ";
    }
    else if (inputLength > length && length <= 5)
    {
        result = result.substr(0, length - 1);
        adjustedwString = result;
        adjustedString = StringHelpers::wstringToString(adjustedwString) + " ";
    }

    return adjustedString;
}

std::string StringHelpers::adjustStringLengthWithString(std::string str, int length, const std::string& fill)
{
    std::size_t inputLength = countCharsWithoutBashCodes(str);

    if (inputLength >= length) {
        return str;
    }

    // Pad fill string after the input string if the string is shorter than length characters
    int fillLength = length - inputLength;
    for (int i = 0; i < fillLength; i++) {
        str += fill;
    }
    return str;
}

std::size_t StringHelpers::countCharsWithoutBashCodes(const std::string& str) {
    // Define regular expression pattern to match ANSI escape codes
    std::regex ansi_escape_code("\033\[[0-9;]+m");

    // Remove ANSI escape codes from the input string
    std::string result = std::regex_replace(str, ansi_escape_code, "");

    // Convert the string to a wide character string
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring wideStr = converter.from_bytes(result);

    // Use a ctype facet to remove accents from the wide character string
    std::locale locale("en_US.UTF8");
    const std::ctype<wchar_t>& ctype = std::use_facet<std::ctype<wchar_t>>(locale);
    for (wchar_t& c : wideStr) {
        c = ctype.widen(ctype.narrow(c, '?'));
    }

    // Convert the wide character string back to a byte string
    result = converter.to_bytes(wideStr);

    // Return the length of the string
    return result.length();
}

bool StringHelpers::containsWord(const std::string& str, const std::string& word) {
    std::istringstream iss(str);
    std::vector<std::string> tokens {
        std::istream_iterator<std::string>{iss},
        std::istream_iterator<std::string>{}
    };
    auto it = std::find(tokens.begin(), tokens.end(), word);
    return it != tokens.end();
}

bool StringHelpers::isBeginningOfWord(const std::string& str, const std::string& word) {
    return str.find(word) == 0;
}

bool StringHelpers::isBeginningOfAnyWords(const std::string& sentence, const std::string& wordToSearch)
{
    std::vector <std::string> words = split(sentence, ' ');
    return std::ranges::any_of(words, [&wordToSearch](const std::string& completeWord) {
        return isBeginningOfWord(completeWord, wordToSearch);
    });
}

bool StringHelpers::containsString(const std::string& str, const std::string& word) {
    return str.find(word) != std::string::npos;
}

std::string StringHelpers::toLower(const std::string& str) {
    std::string lowerStr;
    lowerStr.resize(str.size());
    std::transform(str.begin(), str.end(), lowerStr.begin(), ::tolower);
    return lowerStr;
}

std::string StringHelpers::toUpper(const std::string& str) {
    std::string upperStr;
    upperStr.resize(str.size());
    std::transform(str.begin(), str.end(), upperStr.begin(), ::toupper);
    return upperStr;
}

std::string StringHelpers::filterAlnumAndSpace(const std::string& str) {
    std::string result = str;
    result.erase(std::remove_if(result.begin(), result.end(), [](unsigned char c) {
        return !std::isalnum(c) && !std::isspace(c);
    }), result.end());
    return result;
}

std::vector<std::string> StringHelpers::split(const std::string& basicString, char i)
{
    std::vector<std::string> result;
    std::stringstream ss(basicString);
    std::string item;
    while (std::getline(ss, item, i))
    {
        result.push_back(item);
    }
    return result;
}

bool StringHelpers::isAlnum(const std::string& str)
{
       return std::all_of(str.begin(), str.end(), ::isalnum);
}
