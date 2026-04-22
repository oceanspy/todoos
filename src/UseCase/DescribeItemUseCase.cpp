#include "DescribeItemUseCase.h"
#include "../Actions/Description/ItemTempFile.h"
#include "../Actions/ShowAction/ShowAction.h"
#include "../Describe/DescribeItem.h"
#include "../Describe/DescribeItemEdited.h"
#include "../List/ListItems/ListItemEntity.h"
#include "../List/ListName.h"
#include <filesystem>
#include <unistd.h>

DescribeItemUseCase::DescribeItemUseCase(IOService& ioService,
                                         CommandService& commandService,
                                         ListItemService& listItemService,
                                         ListService& listService,
                                         ConfigService& configService,
                                         ThemeService& themeService,
                                         DescriptionRepository& descriptionRepository,
                                         std::filesystem::path cacheDirPath)
  : ioService(ioService)
  , commandService(commandService)
  , listItemService(listItemService)
  , listService(listService)
  , configService(configService)
  , themeService(themeService)
  , descriptionRepository(descriptionRepository)
  , cacheDirPath(std::move(cacheDirPath))
{
}

void
DescribeItemUseCase::execute(Command& command, ListName& currentList)
{
    std::string itemId = command.getName();
    ListItemEntity listItem = listItemService.find(itemId, currentList);

    std::string existingDescription;
    if (descriptionRepository.exists(itemId, currentList)) {
        existingDescription = descriptionRepository.load(itemId, currentList);
    }

    DescribeItem describeItem = DescribeItem::create(currentList, listItem, existingDescription);
    ItemTempFile tempFile(describeItem, cacheDirPath);
    std::string editFilePath = tempFile.execute();

    std::string destPath = descriptionRepository.getFilePath(itemId, currentList).string();
    std::string pendingError;

    while (true) {
        try {
            DescribeItemEdited edited = DescribeItemEdited::createFromFile(editFilePath, destPath);

            if (edited.asTitle() != *listItem.getValue()) {
                listItemService.edit(itemId, currentList, edited.asTitle());
            }

            if (!edited.hasDescription()) {
                descriptionRepository.remove(itemId, currentList);
            }

            break;
        } catch (std::exception& exception) {
            pendingError = exception.what();
            ItemTempFile::repair(editFilePath, describeItem);
            ItemTempFile::reopen(editFilePath);
        }
    }

    std::filesystem::remove(editFilePath);

    ioService.br();
    ioService.success("Item description " + itemId + " updated.");
    ioService.br();

    ShowAction show(ioService, listService, listItemService, themeService);
    std::vector<ListItemEntity> listItems = listItemService.get(currentList);
    try {
        show.execute(listItems, currentList);
    } catch (std::exception& exception) {
        ioService.br();
        ioService.error(exception.what());
        ioService.br();
    }

    if (!pendingError.empty()) {
        ioService.br();
        ioService.error(pendingError);
        ioService.br();
    }
}
