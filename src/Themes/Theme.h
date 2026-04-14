#ifndef TODOOS_THEME_H
#define TODOOS_THEME_H

#include <iterator>
#include <sstream>
#include <utility>
#include <vector>

#include "../Helpers/DateHelpers.h"
#include "../Helpers/StringHelpers.h"
#include "../IOService/IOService.h"
#include "../List/ListCountSummary.h"
#include "../List/ListItemService.h"
#include "../List/ListName.h"
#include "../List/ListService.h"

class Theme
{
  public:
    Theme(IOService& ioService,
          ListService& listService,
          ListItemService& listItemService,
          int consoleWidth,
          int consoleRowLength)
      : ioService(ioService)
      , listService(listService)
      , listItemService(listItemService)
      , consoleWidth(consoleWidth)
      , consoleRowLength(consoleRowLength) {};

    // Concrete — identical across themes
    void print(ListName& listName, std::vector<ListItemEntity> listItems, bool showListName, bool showTitle);
    void printMultipleList(std::vector<ListName>& listNames, std::vector<ListItemEntity>& listItems);
    void printAList(std::vector<std::string> lines);
    void printFullLine(std::string color = GRAY);
    std::string buildId(const ListItemEntity& listItemEntity);
    std::string buildStatus(const ListItemEntity& listItemEntity);
    std::string buildPriority(const ListItemEntity& listItemEntity);
    std::string buildValue(const ListItemEntity& listItemEntity, const int leftOffset);
    std::string buildDate(const ListItemEntity& listItemEntity);
    std::string buildPriorityCounts(const ListCountSummary& summary);
    std::string buildShortStatsCounts(const ListCountSummary& summary);

    // Pure virtual — theme-specific
    virtual void printListTitle(ListName& listName) = 0;
    virtual void printMultipleListTitles(std::vector<ListName>& listNames) = 0;
    virtual void printATitle(std::string titleLine1, std::string titleLine2) = 0;
    virtual void printStats(ListName& listName) = 0;
    virtual std::string printListRow(const ListItemEntity& listItemEntity, bool printListNameInLine) = 0;
    virtual std::string printListTitleRow() = 0;

    virtual ~Theme() = default;

  protected:
    IOService& ioService;
    ListService& listService;
    ListItemService& listItemService;

    static const int ID_LENGTH = 6;
    static const int PRIORITY_LENGTH = 4;
    static const int STATUS_LENGTH = 20;
    static const int DATE_LENGTH = 25;
    static const int LISTNAME_LENGTH = 12;

    int consoleWidth;
    int consoleRowLength;
    int statsWhenLength = 0;
    int statsCreatedCompletedLength = 0;
    int statsPercentageLength = 0;

    void renderListStatLine(ListName& listName, time_t from, time_t to, std::string name);
    static std::string autoLineBreak(const std::string& value, const int& length, const int leadingSpaces = 0);

  private:
    static std::vector<std::string> splitString(const std::string& str);
};

#endif // TODOOS_THEME_H
