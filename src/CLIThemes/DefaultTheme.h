#ifndef TODOOS_DEFAULTTHEME_H
#define TODOOS_DEFAULTTHEME_H

#include "ThemeAbstract.h"
#include <utility>

class DefaultTheme : public ThemeAbstract
{
  public:
    DefaultTheme(IOService& ioService,
                 ListService& listService,
                 ListItemService& listItemService,
                 int consoleWidth,
                 int consoleRowMaxLength);
    void print(std::string currentList,
               std::string currentListVariant,
               std::vector<ListItemEntity> listItems,
               bool showListName,
               bool showTitle) override;
    void printMultipleList(std::string& listName,
                           std::string& listVariant,
                           std::vector<ListItemEntity>& listItems) override;
    void printStats() override;
    void printATitle(std::string titleLine1, std::string titleLine2) override;
    void printAListTitle(std::vector<std::string> titles, std::vector<int> titleSizes) override;
    void printAList(std::vector<std::string> lines) override;
    void printListName() override;
    std::string buildLine(const ListItemEntity& listItemEntity, bool printListNameInLine) override;
    std::string buildTitle() override;
    std::string buildId(const ListItemEntity& listItemEntity) override;
    std::string buildStatus(const ListItemEntity& listItemEntity) override;
    std::string buildPriority(const ListItemEntity& listItemEntity) override;
    std::string buildValue(const ListItemEntity& listItemEntity, const int leftOffset) override;
    std::string buildDate(const ListItemEntity& listItemEntity) override;
    void printFullLine(std::string color) override;
    ~DefaultTheme() override = default;
    void renderListStatLine(time_t from, time_t to, std::string name);

  private:
    static const int ID_LENGTH = 6;
    static const int PRIORITY_LENGTH = 4;
    static const int STATUS_LENGTH = 20;
    static const int DATE_LENGTH = 25;
    static const int LISTNAME_LENGTH = 12;

    static const int STATS_WHEN_LENGTH = 26;
    static const int STATS_CREATED_COMPLETED_LENGTH = 20;
    static const int STATS_PERCENTAGE_LENGTH = 8;

    int consoleRowLength;
};

#endif // TODOOS_DEFAULTTHEME_H
