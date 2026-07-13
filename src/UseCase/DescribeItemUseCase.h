#pragma once

#include "../Command/Command.h"
#include "../Command/CommandService.h"
#include "../Config/ConfigService.h"
#include "../FileDataStorageRepositories/DescriptionRepository.h"
#include "../IOService/IOService.h"
#include "../List/ListItemService.h"
#include "../List/ListService.h"
#include "../Themes/ThemeService.h"
#include <filesystem>

class DescribeItemUseCase
{
  public:
    DescribeItemUseCase(IOService& ioService,
                        CommandService& commandService,
                        ListItemService& listItemService,
                        ListService& listService,
                        ConfigService& configService,
                        ThemeService& themeService,
                        DescriptionRepository& descriptionRepository,
                        std::filesystem::path cacheDirPath);
    void execute(Command& command, ListName& currentList);

  private:
    IOService& ioService;
    CommandService& commandService;
    ListItemService& listItemService;
    ListService& listService;
    ConfigService& configService;
    ThemeService& themeService;
    DescriptionRepository& descriptionRepository;
    std::filesystem::path cacheDirPath;
};
