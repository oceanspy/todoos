#include "StringHelpers.h"

static std::mt19937&
getGenerator()
{
    static std::mt19937 gen(std::random_device{}());
    return gen;
}

std::string
StringHelpers::randomString(const int len)
{
    static const char alphanum[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                   "abcdefghijklmnopqrstuvwxyz";
    std::uniform_int_distribution<int> dis(0, sizeof(alphanum) - 2);
    std::string tmp_s;
    tmp_s.reserve(len);

    for (int i = 0; i < len; ++i) {
        tmp_s += alphanum[dis(getGenerator())];
    }

    return tmp_s;
}

std::string
StringHelpers::randomAlNumString(const int len)
{
    static const char alphanum[] = "0123456789"
                                   "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                   "abcdefghijklmnopqrstuvwxyz";
    std::uniform_int_distribution<int> dis(0, sizeof(alphanum) - 2);
    std::string tmp_s;
    tmp_s.reserve(len);

    for (int i = 0; i < len; ++i) {
        tmp_s += alphanum[dis(getGenerator())];
    }

    return tmp_s;
}

std::string
StringHelpers::randomLettersLowercase(const int len)
{
    static const char alphanum[] = "abcdefghijklmnopqrstuvwxyz";
    std::uniform_int_distribution<int> dis(0, sizeof(alphanum) - 2);
    std::string tmp_s;
    tmp_s.reserve(len);

    for (int i = 0; i < len; ++i) {
        tmp_s += alphanum[dis(getGenerator())];
    }

    return tmp_s;
}

std::wstring
StringHelpers::stringToWstring(const std::string& str)
{
    if (str.empty())
        return {};
    const char* src = str.c_str();
    std::mbstate_t state{};
    std::size_t len = std::mbsrtowcs(nullptr, &src, 0, &state);
    if (len == static_cast<std::size_t>(-1))
        return {};
    std::wstring wstr(len, L'\0');
    src = str.c_str();
    state = std::mbstate_t{};
    std::mbsrtowcs(wstr.data(), &src, len, &state);
    return wstr;
}

std::string
StringHelpers::wstringToString(const std::wstring& wstr)
{
    if (wstr.empty())
        return {};
    const wchar_t* src = wstr.c_str();
    std::mbstate_t state{};
    std::size_t len = std::wcsrtombs(nullptr, &src, 0, &state);
    if (len == static_cast<std::size_t>(-1))
        return {};
    std::string str(len, '\0');
    src = wstr.c_str();
    state = std::mbstate_t{};
    std::wcsrtombs(str.data(), &src, len, &state);
    return str;
}

std::string
StringHelpers::colorize(const std::string& text, const std::string& color)
{
    return color + text + "\033[0m";
}

std::string
StringHelpers::adjustStringLength(const std::string& str, int length = 10, const char fill)
{
    std::size_t inputLength = countCharsWithoutBashCodes(str);
    std::wstring adjustedwString = StringHelpers::stringToWstring(str);

    // Pad with spaces if the string is shorter than length characters
    if (inputLength < length) {
        adjustedwString.append(length - inputLength, fill);
        return StringHelpers::wstringToString(adjustedwString);
    }

    // Truncate if the string is longer than length characters

    // Remove all bash codes from the string
    // shrink the string
    // TODO: implement a way to put back the bash codes
    std::string adjustedString;
    static const std::regex ansi_escape_code("\033\[[0-9;]+m");
    std::wstring result = StringHelpers::stringToWstring(std::regex_replace(str, ansi_escape_code, ""));

    if (inputLength == length) {
        adjustedwString = result;
        adjustedString = StringHelpers::wstringToString(adjustedwString);
    } else if (inputLength > length && length > 5) {
        result = result.substr(0, length - 3);
        adjustedwString = result;
        adjustedString = StringHelpers::wstringToString(adjustedwString) + "-  ";
    } else if (inputLength > length && length <= 5) {
        result = result.substr(0, length - 1);
        adjustedwString = result;
        adjustedString = StringHelpers::wstringToString(adjustedwString) + " ";
    }

    return adjustedString;
}

std::string
StringHelpers::adjustStringLengthWithString(std::string str, int length, const std::string& fill)
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

std::size_t
StringHelpers::countCharsWithoutBashCodes(const std::string& str)
{
    static const std::regex ansi_escape_code("\033\[[0-9;]+m");
    std::string result = std::regex_replace(str, ansi_escape_code, "");

    std::wstring wideStr = stringToWstring(result);

    std::size_t width = 0;
    for (wchar_t c : wideStr) {
        int w = wcwidth(c);
        width += (w > 0) ? w : (c != L'\0' ? 1 : 0);
    }
    return width;
}

bool
StringHelpers::containsWord(const std::string& str, const std::string& word)
{
    std::istringstream iss(str);
    std::vector<std::string> tokens{ std::istream_iterator<std::string>{ iss }, std::istream_iterator<std::string>{} };
    auto it = std::find(tokens.begin(), tokens.end(), word);
    return it != tokens.end();
}

bool
StringHelpers::isBeginningOfWord(const std::string& str, const std::string& word)
{
    return str.find(word) == 0;
}

bool
StringHelpers::isBeginningOfAnyWords(const std::string& sentence, const std::string& wordToSearch)
{
    std::vector<std::string> words = split(sentence, ' ');
    return std::ranges::any_of(words, [&wordToSearch](const std::string& completeWord) {
        return isBeginningOfWord(completeWord, wordToSearch);
    });
}

bool
StringHelpers::containsString(const std::string& str, const std::string& word)
{
    return str.find(word) != std::string::npos;
}

std::string
StringHelpers::toLower(const std::string& str)
{
    std::string lowerStr;
    lowerStr.resize(str.size());
    std::transform(str.begin(), str.end(), lowerStr.begin(), ::tolower);
    return lowerStr;
}

std::string
StringHelpers::toUpper(const std::string& str)
{
    std::string upperStr;
    upperStr.resize(str.size());
    std::transform(str.begin(), str.end(), upperStr.begin(), ::toupper);
    return upperStr;
}

std::string
StringHelpers::filterAlnumAndSpace(const std::string& str)
{
    std::string result = str;
    result.erase(std::remove_if(result.begin(),
                                result.end(),
                                [](unsigned char c) { return !std::isalnum(c) && !std::isspace(c); }),
                 result.end());
    return result;
}

std::vector<std::string>
StringHelpers::split(const std::string& basicString, char i)
{
    std::vector<std::string> result;
    std::stringstream ss(basicString);
    std::string item;
    while (std::getline(ss, item, i)) {
        result.push_back(item);
    }
    return result;
}

bool
StringHelpers::isAlnum(const std::string& str)
{
    return std::all_of(str.begin(), str.end(), ::isalnum);
}

std::string
StringHelpers::escapeChar(const std::string& str, char charToReplace)
{
    std::string escaped;
    for (char ch : str) {
        if (ch == charToReplace) {
            escaped += "\\";
            escaped += charToReplace;
        } else {
            escaped += ch;
        }
    }
    return escaped;
}

std::string
StringHelpers::vectorToString(std::vector<std::string> arguments)
{
    std::string value;
    for (int i = 0; i < arguments.size(); i++) {
        if (i == 0) {
            value += arguments.at(i);
            continue;
        }
        value += " " + arguments.at(i);
    }
    return value;
}
