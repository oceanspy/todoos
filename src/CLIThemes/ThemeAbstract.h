#ifndef TODOOS_THEMEABSTRACT_H
#define TODOOS_THEMEABSTRACT_H

#include <utility>

#include "../IOService/IOService.h"
#include "../List/ListItemService.h"
#include "../Helpers/DateHelpers.h"
#include "../Helpers/StringHelpers.h"
#include "../List/ListService.h"

class ThemeAbstract {
public:
    ThemeAbstract(IOService& ioService,
                  ListService& listService,
                  ListItemService& listItemService,
                  int consoleWidth
                  ) :
    ioService(ioService),
    listService(listService),
    listItemService(listItemService),
    consoleWidth(consoleWidth)
    {};
    virtual void print(std::string currentList, std::string currentListVariant, std::vector <ListItemEntity> listItems, bool showListName, bool showTitle) = 0;
    virtual void printATitle(std::string titleLine1, std::string titleLine2) = 0;
    virtual void printAListTitle(std::vector <std::string> titles, std::vector <int> titleSizes) = 0;
    virtual void printAList(std::vector <std::string> lines) = 0;
    virtual void printStats() = 0;
    virtual void printListName() = 0;
    virtual std::string buildLine(const ListItemEntity& listItemEntity, bool printListNameInLine) = 0;
    virtual std::string buildTitle() = 0;
    virtual std::string buildId(const ListItemEntity& listItemEntity) = 0;
    virtual std::string buildStatus(const ListItemEntity& listItemEntity) = 0;
    virtual std::string buildPriority(const ListItemEntity& listItemEntity) = 0;
    virtual std::string buildValue(const ListItemEntity& listItemEntity) = 0;
    virtual std::string buildDate(const ListItemEntity& listItemEntity) = 0;
    virtual void printFullLine(std::string color = GRAY) = 0;
    virtual ~ThemeAbstract() = default;
    std::string currentList;
    std::string currentListVariant;
protected:
    IOService ioService;
    ListService& listService;
    ListItemService& listItemService;
    static std::string autoLineBreak(const std::string& value, const int& length, const int leadingSpaces = 0);
    int consoleWidth;
private:
    static std::vector<std::string> splitString(const std::string& str);
};


#endif //TODOOS_THEMEABSTRACT_H
