#include "../../List/ListService.h"
#include "../../FileDataStorage/ConfService.h"
#include "../../FileDataStorage/JSONService.h"
#include "../../FileDataStorageRepositories/ListRepository.h"
#include "../../IOService/IOService.h"
#include "../Mock/MockInit.h"
#include "../Mock/MockInstallation.h"
#include <catch2/catch_test_macros.hpp>
#include <filesystem>

TEST_CASE("ListServiceTest", "[ListService]")
{
    // Create mock objects
    IOService ioService("cli");
    ConfService confService = ConfService(ioService);
    JSONService jsonService = JSONService(ioService);
    std::unique_ptr<FileDataServiceInterface> fileDataStorageServicePtr = std::make_unique<JSONService>(ioService);
    std::unique_ptr<FileDataServiceInterface> fileDataConfigStorageServicePtr =
        std::make_unique<ConfService>(ioService);
    MockInit init(ioService, "_todoos_ListItemServiceTest");
    MockInstallation installation(ioService, jsonService, confService, init);
    std::string tempListNameStr = "tempListName";
    std::string tempListName2Str = "tempList2Name";

    installation.wipe();
    installation.make();
    EventBus bus = EventBus();
    Command command = Command("", {}, {}, "");
    ConfigRepository configRepository(fileDataConfigStorageServicePtr.get(), init.getConfigFilePath());
    ConfigRepository cacheRepository(fileDataConfigStorageServicePtr.get(), init.getCacheFilePath());
    ConfigService configService(ioService, init, configRepository, cacheRepository, command);

    ListRepository listRepository(configService, fileDataStorageServicePtr.get());
    ListService listService(ioService, configService, listRepository, bus);
    ListName listName = listService.createUsedListName();
    ListName listNameArchive = ListName::createVariant(listName, "archive");
    ListName listNameDelete = ListName::createVariant(listName, "delete");
    ListName tempListName = listService.createListName(tempListNameStr);
    ListName tempListNameArchive = ListName::createVariant(tempListName, "archive");
    ListName tempListNameDelete = ListName::createVariant(tempListName, "delete");
    ListName tempListName2 = listService.createListName(tempListName2Str);
    ListName tempListName2Archive = ListName::createVariant(tempListName2, "archive");
    ListName tempListName2Delete = ListName::createVariant(tempListName2, "delete");

    SECTION("get")
    {
        std::vector<ListEntity> lists = listService.get();
        REQUIRE(lists.size() == 2);
        REQUIRE(*lists[0].getName() == tempListName2Str);
        REQUIRE(*lists[1].getName() == tempListNameStr);
    }

    SECTION("find")
    {
        ListEntity listItemEntity = listService.find(tempListNameStr);
        REQUIRE(*listItemEntity.getName() == tempListNameStr);
        REQUIRE(*listItemEntity.getType() == "default");
        REQUIRE(*listItemEntity.getSorting() == "default");
    }

    SECTION("add")
    {
        listService.add("newList", "default", "default");
        ListEntity listItemEntity = listService.find("newList");
        REQUIRE(*listItemEntity.getName() == "newList");
        REQUIRE(*listItemEntity.getType() == "default");
        REQUIRE(*listItemEntity.getSorting() == "default");
    }

    SECTION("edit")
    {
        listService.edit(tempListNameStr, "newList");

        ListEntity listItemEntity = listService.find("newList");
        REQUIRE(*listItemEntity.getName() == "newList");
        REQUIRE(*listItemEntity.getType() == "default");
        REQUIRE(*listItemEntity.getSorting() == "default");

        listService.edit("newList", tempListNameStr);
    }

    SECTION("add & remove")
    {
        std::string list = "newList2";
        listService.add(list, "default", "default");
        ListEntity listItemEntity = listService.find(list);
        REQUIRE(*listItemEntity.getName() == "newList2");
        REQUIRE(*listItemEntity.getType() == "default");
        REQUIRE(*listItemEntity.getSorting() == "default");

        listService.remove(list);

        REQUIRE_THROWS(listService.find(list));
    }

    SECTION("isListExist")
    {
        bool exists = listService.isListExist(tempListNameStr);
        REQUIRE(exists == true);

        bool notexists = listService.isListExist("unknown");
        REQUIRE(notexists == false);
    }

    SECTION("validateListName")
    {
        // must be alphanumeric
        REQUIRE_THROWS(ListService::validateListName(",taust"));
        REQUIRE_THROWS(listService.add("new List", "default", "default"));
        REQUIRE_NOTHROW(ListService::validateListName("taust"));
        REQUIRE_NOTHROW(ListService::validateListName("taust55"));

        // must be 50 char max
        REQUIRE_THROWS(
            ListService::validateListName("aaaaaaaaaabbbbbbbbbbbcccccccccccdddddddddddeeeeeeeeeeffffffffff"));
    }

    SECTION("createListName")
    {
        ListName listName = listService.createListName(tempListNameStr);
        REQUIRE(listName.getName() == tempListNameStr);
        REQUIRE(listName.getVariant() == "default");

        ListName listName2 = listService.createListName(tempListNameStr, "archive");
        REQUIRE(listName2.getName() == tempListNameStr);
        REQUIRE(listName2.getVariant() == "archive");

        ListName listNameArchive = ListName::createVariant(listName, "archive");
        ListName listNameDelete = ListName::createVariant(listName, "delete");
        REQUIRE(listNameArchive.getName() == tempListNameStr);
        REQUIRE(listNameArchive.getVariant() == "archive");
        REQUIRE(listNameDelete.getName() == tempListNameStr);
        REQUIRE(listNameDelete.getVariant() == "delete");

        REQUIRE_THROWS(listService.createListName("unknownList"));
        REQUIRE_THROWS(listService.createListName(tempListNameStr, "truc"));
    }

    SECTION("getAutocompleteLists")
    {
        std::string variant = "default";
        std::string autocompletedList = "temp*";
        std::vector<ListName> listNames = listService.getAutocompletedLists(autocompletedList, variant);

        REQUIRE(listNames.size() == 2);
        REQUIRE(listNames[0].getName() == tempListName2.getName());
        REQUIRE(listNames[1].getName() == tempListName.getName());

        std::string list = "tempList2Name2";
        listService.add(list, "default", "default");
        std::string variantArchive = "archive";
        std::string autocompletedList2 = "tempList2Name*";
        std::vector<ListName> listNames2 = listService.getAutocompletedLists(autocompletedList2, variant);

        REQUIRE(listNames2.size() == 2);
        REQUIRE(listNames[0].getName() == tempListName2.getName());
        REQUIRE(listNames2[1].getName() == list);
    }
}
