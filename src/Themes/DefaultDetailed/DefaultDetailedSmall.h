#ifndef TODOOS_DEFAULTDETAILEDSMALL_H
#define TODOOS_DEFAULTDETAILEDSMALL_H

#include "../ThemeAbstract.h"
#include <utility>

class DefaultDetailedSmall : public ThemeAbstract
{
  public:
    DefaultDetailedSmall(IOService& ioService,
                         ListService& listService,
                         ListItemService& listItemService,
                         int consoleWidth,
                         int consoleRowMaxLength);
    void printListTitle(ListName& listName) override;
    void printATitle(std::string titleLine1, std::string titleLine2) override;
    void printStats(ListName& listName) override;
    std::string buildLine(const ListItemEntity& listItemEntity, bool printListNameInLine) override;
    std::string buildTitle() override;
    ~DefaultDetailedSmall() override = default;
};

#endif // TODOOS_DEFAULTDETAILEDSMALL_H
