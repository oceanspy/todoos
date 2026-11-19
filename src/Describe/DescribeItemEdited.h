#pragma once

#include <string>

// Result of the user editing an item's temp description file: the (possibly new) title
// and the description body. Pure parsing: persisting is the DescriptionRepository's job.
class DescribeItemEdited
{
  public:
    static DescribeItemEdited createFromFile(const std::string& srcPath);

    std::string asTitle() const;
    std::string asDescription() const;
    bool hasDescription() const;

  private:
    DescribeItemEdited(std::string title, std::string description);

    std::string title;
    std::string description;

    static constexpr const char* TITLE_MARKER = "<!-- todoos:title -->";
};
