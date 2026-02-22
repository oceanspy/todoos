#ifndef TODOOS_MOBILETHEME_H
#define TODOOS_MOBILETHEME_H

#include "ThemeAbstract.h"
#include <utility>

class MobileTheme : public ThemeAbstract
{
  public:
    MobileTheme(IOService& ioService,
                ListService& listService,
                ListItemService& listItemService,
                int consoleWidth,
                int consoleRowMaxLength);
    void print(ListName& listName, std::vector<ListItemEntity> listItems, bool showListName, bool showTitle) override;
    void printMultipleList(std::vector<ListName>& listNames, std::vector<ListItemEntity>& listItems) override;
    void printStats(ListName& listName) override;
    void printListName(std::vector<ListName>& listName) override;
    void printATitle(std::string titleLine1, std::string titleLine2) override;
    void printAListTitle(std::vector<std::string> titles, std::vector<int> titleSizes) override;
    void printAList(std::vector<std::string> lines) override;
    std::string buildLine(const ListItemEntity& listItemEntity, bool printListNameInLine) override;
    std::string buildTitle() override;
    std::string buildId(const ListItemEntity& listItemEntity) override;
    std::string buildStatus(const ListItemEntity& listItemEntity) override;
    std::string buildPriority(const ListItemEntity& listItemEntity) override;
    std::string buildValue(const ListItemEntity& listItemEntity, const int leftOffset) override;
    std::string buildDate(const ListItemEntity& listItemEntity) override;
    void printFullLine(std::string color) override;
    ~MobileTheme() override = default;
    void renderListStatLine(ListName& listName, time_t from, time_t to, std::string name);

  private:
    static const int ID_LENGTH = 6;
    static const int PRIORITY_LENGTH = 4;
    static const int STATUS_LENGTH = 20;
    static const int DATE_LENGTH = 25;
    static const int LISTNAME_LENGTH = 12;

    static const int STATS_WHEN_LENGTH = 18;
    static const int STATS_CREATED_COMPLETED_LENGTH = 10;
    static const int STATS_PERCENTAGE_LENGTH = 8;

    int consoleRowLength;
};

#endif // TODOOS_MOBILETHEME_H
