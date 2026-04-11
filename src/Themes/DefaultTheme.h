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
    void printATitle(std::string titleLine1, std::string titleLine2) override;
    void printStats(ListName& listName) override;
    std::string buildLine(const ListItemEntity& listItemEntity, bool printListNameInLine) override;
    std::string buildTitle() override;
    ~DefaultTheme() override = default;
};

#endif // TODOOS_DEFAULTTHEME_H
