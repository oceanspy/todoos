#include "DescribeItemEdited.h"
#include <fstream>
#include <stdexcept>

DescribeItemEdited::DescribeItemEdited(std::string title, std::string description)
  : title(std::move(title))
  , description(std::move(description))
{
}

DescribeItemEdited
DescribeItemEdited::createFromFile(const std::string& srcPath)
{
    std::ifstream src(srcPath);
    if (!src.is_open()) {
        throw std::runtime_error("Could not open temp description file.");
    }

    std::string line;
    bool markerFound = false;
    std::string parsedTitle;
    std::string body;
    bool bodyStarted = false;

    while (std::getline(src, line)) {
        if (!markerFound) {
            if (line == TITLE_MARKER) {
                markerFound = true;
            }
            continue;
        }

        if (parsedTitle.empty()) {
            if (line.rfind("# ", 0) != 0 || line.size() <= 2) {
                throw std::runtime_error("Title must be on the line immediately after the todoos marker.");
            }
            parsedTitle = line.substr(2);
            continue;
        }

        if (!bodyStarted && line.empty()) {
            continue;
        }

        bodyStarted = true;
        body += line + "\n";
    }

    if (!markerFound) {
        throw std::runtime_error("todoos title marker not found — file may be corrupted.");
    }

    if (parsedTitle.empty()) {
        throw std::runtime_error("Title line missing after todoos marker.");
    }

    return DescribeItemEdited(parsedTitle, body);
}

std::string
DescribeItemEdited::asTitle() const
{
    return title;
}

std::string
DescribeItemEdited::asDescription() const
{
    return description;
}

bool
DescribeItemEdited::hasDescription() const
{
    return !description.empty();
}
