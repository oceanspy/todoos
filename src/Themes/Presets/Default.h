#ifndef TODOOS_DEFAULT_H
#define TODOOS_DEFAULT_H

#include "../Theme.h"
#include "../ThemeInterface.h"
#include <ctime>
#include <utility>

class Default
  : public Theme
  , public ThemeInterface
{
  public:
    Default(IOService& ioService,
            ListService& listService,
            ListItemService& listItemService,
            int consoleWidth,
            int consoleRowMaxLength);
    void printListTitle(ListName& listNames) override;
    void printMultipleListTitles(std::vector<ListName>& listNames) override;
    void printATitle(std::string titleLine1, std::string titleLine2) override;
    void printStats(ListName& listName) override;
    std::string printListRow(const ListItemEntity& listItemEntity, bool printListNameInLine) override;
    std::string printListTitleRow() override;
    ~Default() override = default;

  private:
    int consoleRowMaxLength;
    std::string buildListTitle(ListName& listName);
    std::string buildListLastUpdate(const time_t& time);
};

#endif // TODOOS_DEFAULT_H
