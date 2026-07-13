#pragma once

#include <string>

class DescribeItemEdited
{
  public:
    static DescribeItemEdited createFromFile(const std::string& srcPath, const std::string& destPath);

    std::string asTitle() const;
    bool hasDescription() const;

  private:
    DescribeItemEdited(std::string title, bool hasDescription);

    std::string title;
    bool descriptionWritten;

    static constexpr const char* TITLE_MARKER = "<!-- todoos:title -->";
};
