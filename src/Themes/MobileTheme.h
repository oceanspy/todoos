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
    void printListName(std::vector<ListName>& listNames) override;
    void printATitle(std::string titleLine1, std::string titleLine2) override;
    void printStats(ListName& listName) override;
    std::string buildLine(const ListItemEntity& listItemEntity, bool printListNameInLine) override;
    std::string buildTitle() override;
    ~MobileTheme() override = default;
};

#endif // TODOOS_MOBILETHEME_H
