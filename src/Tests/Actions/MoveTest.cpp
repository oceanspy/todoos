#include "../../Actions/MoveAction/DuplicateItemAction.h"
#include "../../Actions/MoveAction/MoveItemAction.h"
#include "../../Command/CommandOption.h"
#include "../../Command/CommandRegistry.h"
#include "../../Command/CommandService.h"
#include "../../Config/ConfigService.h"
#include "../../Describe/DescriptionService.h"
#include "../../Events/EventBus.h"
#include "../../FileDataStorageRepositories/ConfigRepository.h"
#include "../../FileDataStorageRepositories/ListItemRepository.h"
#include "../../FileDataStorageRepositories/ListRepository.h"
#include "../../FileDataStorageRepositories/DescriptionRepository.h"
#include "../../List/ListItemService.h"
#include "../../List/ListItems/PriorityService.h"
#include "../../List/ListItems/StatusService.h"
#include "../../List/ListService.h"
#include "../../Serializers/ConfSerializer.h"
#include "../../Serializers/JsonSerializer.h"
#include "../Mock/MockAppInitialization.h"
#include "../Mock/MockAppInstallation.h"
#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <fstream>
#include <sstream>

TEST_CASE("Move action", "[Move]")
{
    IOService ioService("cli");
    ConfSerializer confService(ioService);
    JsonSerializer jsonService(ioService);
    std::unique_ptr<DataSerializerInterface> storagePtr = std::make_unique<JsonSerializer>(ioService);
    std::unique_ptr<DataSerializerInterface> configStoragePtr = std::make_unique<ConfSerializer>(ioService);

    MockAppInitialization init(ioService, "_todoos_MoveActionTest");
    MockAppInstallation installation(ioService, jsonService, confService, init);
    installation.wipe();
    installation.make();

    EventBus bus;
    CommandRegistry commandList;
    CommandOption commandOption;
    CommandService commandService(commandList, commandOption);

    ConfigRepository configRepository(configStoragePtr.get(), init.getConfigFilePath());
    ConfigRepository cacheRepository(configStoragePtr.get(), init.getCacheFilePath());

    PriorityService priorityService;
    StatusService statusService;

    // ---- MoveItemAction — move-to ------------------------------------------------------

    SECTION("move-to moves item to target list")
    {
        Command command("move-to", { "tempList2Name", "aaaa" }, {}, "move-to tempList2Name aaaa");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        DescriptionRepository descriptionRepository(configService.getDescriptionsDirPath());

        ListItemService listItemService(ioService, configService, listItemRepository, descriptionRepository, priorityService, statusService);
        std::unique_ptr<DataSerializerInterface> listStoragePtr = std::make_unique<JsonSerializer>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ListName listName = listService.createUsedListName();

        DescriptionService descriptionService(init.getCacheDirPath(), descriptionRepository);
        MoveItemAction move(ioService, listService, listItemService, descriptionService);
        REQUIRE_NOTHROW(move.execute(command, listName));

        std::vector<ListItemEntity> items = listItemService.get(listName);
        REQUIRE(items.size() == 1);
        REQUIRE(*items[0].getId() == "bbbb");

        installation.wipe();
        installation.make();
    }

    SECTION("move-to with too few arguments does not throw")
    {
        Command command("move-to", { "tempList2Name" }, {}, "move-to tempList2Name");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        DescriptionRepository descriptionRepository(configService.getDescriptionsDirPath());

        ListItemService listItemService(ioService, configService, listItemRepository, descriptionRepository, priorityService, statusService);
        std::unique_ptr<DataSerializerInterface> listStoragePtr = std::make_unique<JsonSerializer>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ListName listName = listService.createUsedListName();

        DescriptionService descriptionService(init.getCacheDirPath(), descriptionRepository);
        MoveItemAction move(ioService, listService, listItemService, descriptionService);
        REQUIRE_NOTHROW(move.execute(command, listName));
    }

    SECTION("move-to with empty arguments does not throw")
    {
        Command command("move-to", {}, {}, "move-to");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        DescriptionRepository descriptionRepository(configService.getDescriptionsDirPath());

        ListItemService listItemService(ioService, configService, listItemRepository, descriptionRepository, priorityService, statusService);
        std::unique_ptr<DataSerializerInterface> listStoragePtr = std::make_unique<JsonSerializer>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ListName listName = listService.createUsedListName();

        DescriptionService descriptionService(init.getCacheDirPath(), descriptionRepository);
        MoveItemAction move(ioService, listService, listItemService, descriptionService);
        REQUIRE_NOTHROW(move.execute(command, listName));
    }

    // ---- MoveItemAction — description ---------------------------------------------------

    SECTION("move-to moves the item description to the target list")
    {
        Command command("move-to", { "tempList2Name", "aaaa" }, {}, "move-to tempList2Name aaaa");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        DescriptionRepository descriptionRepository(configService.getDescriptionsDirPath());
        DescriptionService descriptionService(init.getCacheDirPath(), descriptionRepository);

        ListItemService listItemService(ioService, configService, listItemRepository, descriptionRepository, priorityService, statusService);
        std::unique_ptr<DataSerializerInterface> listStoragePtr = std::make_unique<JsonSerializer>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ListName listName = listService.createUsedListName();
        ListName newListName = listService.createListName("tempList2Name");

        std::filesystem::path oldPath = descriptionRepository.getFilePath("aaaa", listName);
        std::filesystem::create_directories(oldPath.parent_path());
        std::ofstream(oldPath) << "Some description.\n";

        MoveItemAction move(ioService, listService, listItemService, descriptionService);
        REQUIRE(move.execute(command, listName) == "tempList2Name");

        REQUIRE_FALSE(descriptionRepository.exists("aaaa", listName));
        REQUIRE(descriptionRepository.exists("aaaa", newListName));
        std::ifstream movedFile(descriptionRepository.getFilePath("aaaa", newListName));
        std::ostringstream movedContent;
        movedContent << movedFile.rdbuf();
        REQUIRE(movedContent.str() == "Some description.\n");

        std::vector<ListItemEntity> movedItems = listItemService.get(newListName);
        auto movedItem = std::ranges::find_if(movedItems, [](const ListItemEntity& item) { return *item.getId() == "aaaa"; });
        REQUIRE(movedItem != movedItems.end());
        REQUIRE(*movedItem->hasDescription());

        installation.wipe();
        installation.make();
    }

    SECTION("move-to of an item without description does not create one in the target list")
    {
        Command command("move-to", { "tempList2Name", "aaaa" }, {}, "move-to tempList2Name aaaa");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        DescriptionRepository descriptionRepository(configService.getDescriptionsDirPath());
        DescriptionService descriptionService(init.getCacheDirPath(), descriptionRepository);

        ListItemService listItemService(ioService, configService, listItemRepository, descriptionRepository, priorityService, statusService);
        std::unique_ptr<DataSerializerInterface> listStoragePtr = std::make_unique<JsonSerializer>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ListName listName = listService.createUsedListName();
        ListName newListName = listService.createListName("tempList2Name");

        MoveItemAction move(ioService, listService, listItemService, descriptionService);
        REQUIRE(move.execute(command, listName) == "tempList2Name");

        REQUIRE_FALSE(descriptionRepository.exists("aaaa", newListName));
        REQUIRE(listItemService.get(newListName).size() == 3);

        installation.wipe();
        installation.make();
    }

    SECTION("move-to of several items only moves the existing descriptions")
    {
        Command command("move-to", { "tempList2Name", "aaaa", "bbbb" }, {}, "move-to tempList2Name aaaa bbbb");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        DescriptionRepository descriptionRepository(configService.getDescriptionsDirPath());
        DescriptionService descriptionService(init.getCacheDirPath(), descriptionRepository);

        ListItemService listItemService(ioService, configService, listItemRepository, descriptionRepository, priorityService, statusService);
        std::unique_ptr<DataSerializerInterface> listStoragePtr = std::make_unique<JsonSerializer>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ListName listName = listService.createUsedListName();
        ListName newListName = listService.createListName("tempList2Name");

        std::filesystem::path oldPath = descriptionRepository.getFilePath("bbbb", listName);
        std::filesystem::create_directories(oldPath.parent_path());
        std::ofstream(oldPath) << "Description of bbbb.\n";

        MoveItemAction move(ioService, listService, listItemService, descriptionService);
        REQUIRE(move.execute(command, listName) == "tempList2Name");

        REQUIRE_FALSE(descriptionRepository.exists("aaaa", newListName));
        REQUIRE_FALSE(descriptionRepository.exists("bbbb", listName));
        REQUIRE(descriptionRepository.exists("bbbb", newListName));
        REQUIRE(listItemService.get(listName).empty());

        installation.wipe();
        installation.make();
    }

    SECTION("move-to overwriting an item removes the target description when the source has none")
    {
        std::map<std::string, std::string> forceOpt = { { "force", "" } };
        Command command("move-to", { "tempList2Name", "aaaa" }, forceOpt, "move-to --force tempList2Name aaaa");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        DescriptionRepository descriptionRepository(configService.getDescriptionsDirPath());
        DescriptionService descriptionService(init.getCacheDirPath(), descriptionRepository);

        ListItemService listItemService(ioService, configService, listItemRepository, descriptionRepository, priorityService, statusService);
        std::unique_ptr<DataSerializerInterface> listStoragePtr = std::make_unique<JsonSerializer>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ListName listName = listService.createUsedListName();
        ListName targetListName = listService.createListName("tempList2Name");

        // target list already has an item aaaa with its own description, source aaaa has none
        listItemService.copy("aaaa", listName, targetListName);
        std::filesystem::path targetPath = descriptionRepository.getFilePath("aaaa", targetListName);
        std::filesystem::create_directories(targetPath.parent_path());
        std::ofstream(targetPath) << "Stale description.\n";

        MoveItemAction move(ioService, listService, listItemService, descriptionService);
        REQUIRE(move.execute(command, listName) == "tempList2Name");

        REQUIRE_FALSE(descriptionRepository.exists("aaaa", targetListName));
        REQUIRE_FALSE(*listItemService.find("aaaa", targetListName).hasDescription());

        installation.wipe();
        installation.make();
    }

    SECTION("move-to overwriting an item replaces the target description with the source one")
    {
        std::map<std::string, std::string> forceOpt = { { "force", "" } };
        Command command("move-to", { "tempList2Name", "aaaa" }, forceOpt, "move-to --force tempList2Name aaaa");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        DescriptionRepository descriptionRepository(configService.getDescriptionsDirPath());
        DescriptionService descriptionService(init.getCacheDirPath(), descriptionRepository);

        ListItemService listItemService(ioService, configService, listItemRepository, descriptionRepository, priorityService, statusService);
        std::unique_ptr<DataSerializerInterface> listStoragePtr = std::make_unique<JsonSerializer>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ListName listName = listService.createUsedListName();
        ListName targetListName = listService.createListName("tempList2Name");

        // target list already has an item aaaa with its own description, source aaaa has none
        listItemService.copy("aaaa", listName, targetListName);
        std::filesystem::path targetPath = descriptionRepository.getFilePath("aaaa", targetListName);
        std::filesystem::create_directories(targetPath.parent_path());
        std::ofstream(targetPath) << "Stale description.\n";

        std::filesystem::path sourcePath = descriptionRepository.getFilePath("aaaa", listName);
        std::filesystem::create_directories(sourcePath.parent_path());
        std::ofstream(sourcePath) << "Source description.\n";

        MoveItemAction move(ioService, listService, listItemService, descriptionService);
        REQUIRE(move.execute(command, listName) == "tempList2Name");

        REQUIRE_FALSE(descriptionRepository.exists("aaaa", listName));
        std::ifstream movedFile(descriptionRepository.getFilePath("aaaa", targetListName));
        std::ostringstream movedContent;
        movedContent << movedFile.rdbuf();
        REQUIRE(movedContent.str() == "Source description.\n");

        installation.wipe();
        installation.make();
    }

    // ---- DuplicateItemAction -----------------------------------------------------------

    SECTION("duplicate increases item count")
    {
        Command command("duplicate", { "aaaa" }, {}, "duplicate aaaa");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        DescriptionRepository descriptionRepository(configService.getDescriptionsDirPath());
        DescriptionService descriptionService(init.getCacheDirPath(), descriptionRepository);

        ListItemService listItemService(ioService, configService, listItemRepository, descriptionRepository, priorityService, statusService);
        std::unique_ptr<DataSerializerInterface> listStoragePtr = std::make_unique<JsonSerializer>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ListName listName = listService.createUsedListName();

        DuplicateItemAction move(ioService, listService, listItemService, descriptionService);
        REQUIRE_NOTHROW(move.execute(command, listName));

        std::vector<ListItemEntity> items = listItemService.get(listName);
        REQUIRE(items.size() == 3);

        installation.wipe();
        installation.make();
    }

    SECTION("duplicate copies the item description to the new item")
    {
        Command command("duplicate", { "aaaa" }, {}, "duplicate aaaa");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        DescriptionRepository descriptionRepository(configService.getDescriptionsDirPath());
        DescriptionService descriptionService(init.getCacheDirPath(), descriptionRepository);

        ListItemService listItemService(ioService, configService, listItemRepository, descriptionRepository, priorityService, statusService);
        std::unique_ptr<DataSerializerInterface> listStoragePtr = std::make_unique<JsonSerializer>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ListName listName = listService.createUsedListName();

        std::filesystem::path sourcePath = descriptionRepository.getFilePath("aaaa", listName);
        std::filesystem::create_directories(sourcePath.parent_path());
        std::ofstream(sourcePath) << "Some description.\n";

        DuplicateItemAction duplicate(ioService, listService, listItemService, descriptionService);
        REQUIRE(duplicate.execute(command, listName) == listName.getName());

        std::vector<ListItemEntity> items = listItemService.get(listName);
        REQUIRE(items.size() == 3);
        auto newItem = std::ranges::find_if(items, [](const ListItemEntity& item) {
            return *item.getId() != "aaaa" && *item.getId() != "bbbb";
        });
        REQUIRE(newItem != items.end());
        REQUIRE(*newItem->hasDescription());

        REQUIRE(descriptionRepository.exists("aaaa", listName));
        std::ifstream duplicatedFile(descriptionRepository.getFilePath(*newItem->getId(), listName));
        std::ostringstream duplicatedContent;
        duplicatedContent << duplicatedFile.rdbuf();
        REQUIRE(duplicatedContent.str() == "Some description.\n");

        installation.wipe();
        installation.make();
    }

    SECTION("duplicate of an item without description does not create one")
    {
        Command command("duplicate", { "aaaa" }, {}, "duplicate aaaa");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        DescriptionRepository descriptionRepository(configService.getDescriptionsDirPath());
        DescriptionService descriptionService(init.getCacheDirPath(), descriptionRepository);

        ListItemService listItemService(ioService, configService, listItemRepository, descriptionRepository, priorityService, statusService);
        std::unique_ptr<DataSerializerInterface> listStoragePtr = std::make_unique<JsonSerializer>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ListName listName = listService.createUsedListName();

        DuplicateItemAction duplicate(ioService, listService, listItemService, descriptionService);
        REQUIRE(duplicate.execute(command, listName) == listName.getName());

        std::vector<ListItemEntity> items = listItemService.get(listName);
        REQUIRE(items.size() == 3);
        REQUIRE(descriptionRepository.getIds(listName).empty());

        installation.wipe();
        installation.make();
    }
}
