#ifndef TODOOS_DEFAULTDETAILEDMEDIUM_H
#define TODOOS_DEFAULTDETAILEDMEDIUM_H

#include "../ThemeAbstract.h"
#include <utility>

class DefaultDetailedMedium : public ThemeAbstract
{
  public:
    DefaultDetailedMedium(IOService& ioService,
                          ListService& listService,
                          ListItemService& listItemService,
                          int consoleWidth,
                          int consoleRowMaxLength);
    void printListTitle(ListName& listName) override;
    void printATitle(std::string titleLine1, std::string titleLine2) override;
    void printStats(ListName& listName) override;
    std::string buildLine(const ListItemEntity& listItemEntity, bool printListNameInLine) override;
    std::string buildTitle() override;
    ~DefaultDetailedMedium() override = default;

  private:
    std::string buildPriorityCounts(const ListCountSummary& summary);
};

#endif // TODOOS_DEFAULTDETAILEDMEDIUM_H
