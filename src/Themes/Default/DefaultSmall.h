#ifndef TODOOS_DEFAULTSMALL_H
#define TODOOS_DEFAULTSMALL_H

#include "../ThemeAbstract.h"
#include <utility>

class DefaultSmall : public ThemeAbstract
{
  public:
    DefaultSmall(IOService& ioService,
                 ListService& listService,
                 ListItemService& listItemService,
                 int consoleWidth,
                 int consoleRowMaxLength);
    void printListTitle(ListName& listName) override;
    void printATitle(std::string titleLine1, std::string titleLine2) override;
    void printStats(ListName& listName) override;
    std::string buildLine(const ListItemEntity& listItemEntity, bool printListNameInLine) override;
    std::string buildTitle() override;
    ~DefaultSmall() override = default;
};

#endif // TODOOS_DEFAULTSMALL_H
