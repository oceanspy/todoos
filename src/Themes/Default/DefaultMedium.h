#ifndef TODOOS_DEFAULTMEDIUM_H
#define TODOOS_DEFAULTMEDIUM_H

#include "../ThemeAbstract.h"
#include <utility>

class DefaultMedium : public ThemeAbstract
{
  public:
    DefaultMedium(IOService& ioService,
                  ListService& listService,
                  ListItemService& listItemService,
                  int consoleWidth,
                  int consoleRowMaxLength);
    void printListTitle(ListName& listName) override;
    void printATitle(std::string titleLine1, std::string titleLine2) override;
    void printStats(ListName& listName) override;
    std::string printListRow(const ListItemEntity& listItemEntity, bool printListNameInLine) override;
    std::string printListTitleRow() override;
    ~DefaultMedium() override = default;

  private:
    std::string buildListTitle(ListName& listName);
    std::string buildListLastUpdate(const time_t& time);
    std::string buildPriorityCounts(const ListCountSummary& summary);
};

#endif // TODOOS_DEFAULTMEDIUM_H
