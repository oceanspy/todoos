#include "DescribeItemEdited.h"
#include <filesystem>
#include <fstream>
#include <stdexcept>

DescribeItemEdited::DescribeItemEdited(std::string title, bool hasDescription)
  : title(std::move(title))
  , descriptionWritten(hasDescription)
{
}

DescribeItemEdited
DescribeItemEdited::createFromFile(const std::string& srcPath, const std::string& destPath)
{
    std::ifstream src(srcPath);
    if (!src.is_open()) {
        throw std::runtime_error("Could not open temp description file.");
    }

    std::string line;
    bool markerFound = false;
    std::string parsedTitle;
    std::ofstream dest;
    bool wroteAnyLine = false;
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
            std::filesystem::create_directories(std::filesystem::path(destPath).parent_path());
            dest.open(destPath);
            if (!dest.is_open()) {
                throw std::runtime_error("Could not open destination description file.");
            }
            continue;
        }

        if (!bodyStarted && line.empty()) {
            continue;
        }

        bodyStarted = true;
        dest << line << "\n";
        wroteAnyLine = true;
    }

    if (!markerFound) {
        throw std::runtime_error("todoos title marker not found — file may be corrupted.");
    }

    if (parsedTitle.empty()) {
        throw std::runtime_error("Title line missing after todoos marker.");
    }

    dest.close();

    if (!wroteAnyLine && std::filesystem::exists(destPath)) {
        std::filesystem::remove(destPath);
    }

    return DescribeItemEdited(parsedTitle, wroteAnyLine);
}

std::string
DescribeItemEdited::asTitle() const
{
    return title;
}

bool
DescribeItemEdited::hasDescription() const
{
    return descriptionWritten;
}
