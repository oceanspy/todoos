#include "ThemeAbstract.h"

std::string
ThemeAbstract::autoLineBreak(const std::string& value, const int& length, const int leadingSpaces)
{
    std::string br = "\n";
    br += StringHelpers::adjustStringLength("", leadingSpaces);
    std::string currentLine;
    std::vector<std::string> lines;

    // Break the string into words
    std::vector<std::string> words = splitString(value);

    // Loop through the words and add them to the currentLine
    for (int i = 0; i < words.size(); i++) {
        if (currentLine.length() + words[i].length() >= length) {
            // if word is longer than the line, break it
            if (words[i].length() > length) {
                int j = 0;
                while (j < words[i].length()) {
                    // If current line is full, add it to the lines vector
                    if (currentLine.length() >= length) {
                        currentLine += br;
                        lines.push_back(currentLine);
                        currentLine = "";
                    }

                    // Add the next character to the current line
                    currentLine += words[i][j];

                    // If the current line is full, add it to the lines vector
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

    // Add the last line to the lines vector
    lines.push_back(StringHelpers::adjustStringLength(currentLine, length));

    // Join the lines into a single string
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

    // Using stream iterators to extract words from the string
    std::copy(
        std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>(), std::back_inserter(tokens));

    return tokens;
}
