#include "../../Config/ConfigService.h"
#include "../../FileStorage/FileStorageService.h"
#include "../../Help/HelpPrinter.h"
#include "../../IOService/IOService.h"
#include "../../List/ListItemService.h"
#include "../../List/ListService.h"
#include "../../Serializers/ConfSerializer.h"
#include "../Mock/MockAppInitialization.h"
#include "../Mock/MockAppInstallation.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("FileStorage", "[moveFileTo]")
{

    IOService ioService("cli");
    ConfSerializer confService = ConfSerializer(ioService);
    JsonSerializer jsonService = JsonSerializer(ioService);
    std::unique_ptr<DataSerializerInterface> fileDataStorageServicePtr = std::make_unique<JsonSerializer>(ioService);
    std::unique_ptr<DataSerializerInterface> fileDataConfigStorageServicePtr =
        std::make_unique<ConfSerializer>(ioService);
    MockAppInitialization init(ioService, "_todoos_FileStorageTest");
    MockAppInstallation installation(ioService, jsonService, confService, init);
    Command command = Command("commands", {}, {}, "commands");
    ConfigRepository configRepository(fileDataConfigStorageServicePtr.get(), init.getConfigFilePath());
    ConfigRepository cacheRepository(fileDataConfigStorageServicePtr.get(), init.getCacheFilePath());
    ConfigService configService(ioService, init, configRepository, cacheRepository, command);
    FileStorageService fileStorageService(ioService, configService);
    EventBus bus = EventBus();

    installation.wipe();
    installation.make();

    PriorityService priorityService = PriorityService();
    StatusService statusService = StatusService();
    ListItemRepository listItemRepository(
        configService, fileDataStorageServicePtr.get(), priorityService, statusService);
    ListItemService listItemService(ioService, configService, listItemRepository, priorityService, statusService);
    ListRepository listRepository(configService, fileDataStorageServicePtr.get());
    ListService listService(ioService, configService, listRepository, bus);
    ListName listName = listService.createListName("tempListName");

    SECTION("moveFileTo test")
    {
        std::vector<ListItemEntity> listItems = listItemService.get(listName);
        REQUIRE(listItems.size() == 2);

        REQUIRE_NOTHROW(fileStorageService.moveFileTo("tempListName", "tempListName-new"));
        REQUIRE_NOTHROW(listService.edit("tempListName", "tempListName-new"));

        ListEntity list = listService.find("tempListName-new");

        REQUIRE_NOTHROW(listService.find("tempListName-new"));
        REQUIRE_THROWS(listService.find("tempListName"));

        ListName listNameNew = listService.createListName("tempListName-new");
        std::vector<ListItemEntity> listItems2 = listItemService.get(listNameNew);
        REQUIRE(listItems2.size() == 2);
    }

    SECTION("createNewListFile test")
    {
        installation.wipe();
        installation.make();

        std::vector<ListItemEntity> listItems = listItemService.get(listName);
        REQUIRE(listItems.size() == 2);

        REQUIRE_NOTHROW(fileStorageService.createNewListFile("tempListName-new-create"));
        REQUIRE_NOTHROW(listService.add("tempListName-new-create"));

        ListEntity list = listService.find("tempListName-new-create");

        REQUIRE_NOTHROW(listService.find("tempListName-new-create"));

        ListName listNameNew = listService.createListName("tempListName-new-create");
        listItemService.add(listNameNew, "auto");
        std::vector<ListItemEntity> listItems2 = listItemService.get(listNameNew);
        REQUIRE(listItems2.size() == 1);
    }
}
