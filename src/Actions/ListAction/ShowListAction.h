#ifndef SHOWLISTACTION_H
#define SHOWLISTACTION_H

#include "../../Command/Command.h"
#include "../../Config/ConfigService.h"
#include "../../IOService/IOService.h"
#include "../../List/ListService.h"
#include "../../Themes/ThemeService.h"

class ShowListAction
{
  public:
    ShowListAction(IOService& ioService,
                   ListService& listService,
                   ConfigService& configService,
                   ThemeService& themeService);
    void execute(Command& command);

  private:
    IOService& ioService;
    ListService& listService;
    ConfigService& configService;
    ThemeService& themeService;
    std::string buildName(const ListEntity& listEntity);
    static std::string addSpaceToListName(std::string listName);
};

#endif // SHOWLISTACTION_H
