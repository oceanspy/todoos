#ifndef TODOOS_DEFAULTDETAILED_H
#define TODOOS_DEFAULTDETAILED_H

#include "../ThemeAbstract.h"
#include <utility>

class Default : public ThemeAbstract
{
  public:
    Default(IOService& ioService,
            ListService& listService,
            ListItemService& listItemService,
            int consoleWidth,
            int consoleRowMaxLength);
    void printListTitle(ListName& listNames) override;
    void printATitle(std::string titleLine1, std::string titleLine2) override;
    void printStats(ListName& listName) override;
    std::string buildLine(const ListItemEntity& listItemEntity, bool printListNameInLine) override;
    std::string buildTitle() override;
    ~Default() override = default;
};

#endif // TODOOS_DEFAULTDETAILED_H
