#ifndef TODOOS_LEGACY_H
#define TODOOS_LEGACY_H

#include "../Theme.h"
#include "../ThemeInterface.h"
#include <ctime>
#include <utility>

class Legacy
  : public Theme
  , public ThemeInterface
{
  public:
    Legacy(IOService& ioService,
           ListService& listService,
           ListItemService& listItemService,
           int consoleWidth,
           int consoleRowMaxLength);
    void printListTitle(ListName& listName) override;
    void printMultipleListTitles(std::vector<ListName>& listNames) override;
    void printATitle(std::string titleLine1, std::string titleLine2) override;
    void printStats(ListName& listName) override;
    std::string printListRow(const ListItemEntity& listItemEntity, bool printListNameInLine) override;
    std::string printListTitleRow() override;
    ~Legacy() override = default;

  private:
    int consoleRowMaxLength;
    std::string buildListTitle(ListName& listName);
    void renderTitleBox(ListName& displayName, ListCountSummary& summary);
};

#endif // TODOOS_LEGACY_H
