#include "../../Actions/MoveAction/CopyItemAction.h"
#include "../../Actions/MoveAction/DuplicateItemAction.h"
#include "../../Actions/MoveAction/MoveItemAction.h"
#include "../../Command/CommandOption.h"
#include "../../Command/CommandRegistry.h"
#include "../../Command/CommandService.h"
#include "../../Config/ConfigService.h"
#include "../../Describe/DescriptionService.h"
#include "../../Events/EventBus.h"
#include "../../FileDataStorageRepositories/ConfigRepository.h"
#include "../../FileDataStorageRepositories/DescriptionRepository.h"
#include "../../FileDataStorageRepositories/ListItemRepository.h"
#include "../../FileDataStorageRepositories/ListRepository.h"
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

    // ---- MoveItemAction — move ------------------------------------------------------

    SECTION("move moves item to target list")
    {
        Command command("move", { "aaaa" }, { { "to", "tempList2Name" } }, "move aaaa --to tempList2Name");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        DescriptionRepository descriptionRepository(configService.getDescriptionsDirPath());

        ListItemService listItemService(
            ioService, configService, listItemRepository, descriptionRepository, priorityService, statusService);
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

    SECTION("move with --to but no ID does not throw")
    {
        Command command("move", {}, { { "to", "tempList2Name" } }, "move --to tempList2Name");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        DescriptionRepository descriptionRepository(configService.getDescriptionsDirPath());

        ListItemService listItemService(
            ioService, configService, listItemRepository, descriptionRepository, priorityService, statusService);
        std::unique_ptr<DataSerializerInterface> listStoragePtr = std::make_unique<JsonSerializer>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ListName listName = listService.createUsedListName();

        DescriptionService descriptionService(init.getCacheDirPath(), descriptionRepository);
        MoveItemAction move(ioService, listService, listItemService, descriptionService);
        REQUIRE_NOTHROW(move.execute(command, listName));
    }

    SECTION("move with empty arguments does not throw")
    {
        Command command("move", {}, {}, "move");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        DescriptionRepository descriptionRepository(configService.getDescriptionsDirPath());

        ListItemService listItemService(
            ioService, configService, listItemRepository, descriptionRepository, priorityService, statusService);
        std::unique_ptr<DataSerializerInterface> listStoragePtr = std::make_unique<JsonSerializer>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ListName listName = listService.createUsedListName();

        DescriptionService descriptionService(init.getCacheDirPath(), descriptionRepository);
        MoveItemAction move(ioService, listService, listItemService, descriptionService);
        REQUIRE_NOTHROW(move.execute(command, listName));
    }

    SECTION("move without --to does nothing, even with the old positional list argument")
    {
        Command command("move", { "tempList2Name", "aaaa" }, {}, "move tempList2Name aaaa");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        DescriptionRepository descriptionRepository(configService.getDescriptionsDirPath());

        ListItemService listItemService(
            ioService, configService, listItemRepository, descriptionRepository, priorityService, statusService);
        std::unique_ptr<DataSerializerInterface> listStoragePtr = std::make_unique<JsonSerializer>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ListName listName = listService.createUsedListName();

        DescriptionService descriptionService(init.getCacheDirPath(), descriptionRepository);
        MoveItemAction move(ioService, listService, listItemService, descriptionService);
        std::string result;
        REQUIRE_NOTHROW(result = move.execute(command, listName));
        REQUIRE(result.empty());
        REQUIRE(listItemService.get(listName).size() == 2);
    }

    SECTION("move with an empty --to does nothing")
    {
        Command command("move", { "aaaa" }, { { "to", "" } }, "move aaaa --to");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        DescriptionRepository descriptionRepository(configService.getDescriptionsDirPath());

        ListItemService listItemService(
            ioService, configService, listItemRepository, descriptionRepository, priorityService, statusService);
        std::unique_ptr<DataSerializerInterface> listStoragePtr = std::make_unique<JsonSerializer>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ListName listName = listService.createUsedListName();

        DescriptionService descriptionService(init.getCacheDirPath(), descriptionRepository);
        MoveItemAction move(ioService, listService, listItemService, descriptionService);
        std::string result;
        REQUIRE_NOTHROW(result = move.execute(command, listName));
        REQUIRE(result.empty());
        REQUIRE(listItemService.get(listName).size() == 2);
    }

    SECTION("move --to the current list does nothing")
    {
        Command command("move", { "aaaa" }, { { "to", "tempListName" } }, "move aaaa --to tempListName");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        DescriptionRepository descriptionRepository(configService.getDescriptionsDirPath());

        ListItemService listItemService(
            ioService, configService, listItemRepository, descriptionRepository, priorityService, statusService);
        std::unique_ptr<DataSerializerInterface> listStoragePtr = std::make_unique<JsonSerializer>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ListName listName = listService.createUsedListName();

        DescriptionService descriptionService(init.getCacheDirPath(), descriptionRepository);
        MoveItemAction move(ioService, listService, listItemService, descriptionService);
        std::string result;
        REQUIRE_NOTHROW(result = move.execute(command, listName));
        REQUIRE(result.empty());
        REQUIRE(listItemService.get(listName).size() == 2);
    }

    // ---- MoveItemAction — description ---------------------------------------------------

    SECTION("move moves the item description to the target list")
    {
        Command command("move", { "aaaa" }, { { "to", "tempList2Name" } }, "move aaaa --to tempList2Name");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        DescriptionRepository descriptionRepository(configService.getDescriptionsDirPath());
        DescriptionService descriptionService(init.getCacheDirPath(), descriptionRepository);

        ListItemService listItemService(
            ioService, configService, listItemRepository, descriptionRepository, priorityService, statusService);
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
        auto movedItem =
            std::ranges::find_if(movedItems, [](const ListItemEntity& item) { return *item.getId() == "aaaa"; });
        REQUIRE(movedItem != movedItems.end());
        REQUIRE(*movedItem->hasDescription());

        installation.wipe();
        installation.make();
    }

    SECTION("move of an item without description does not create one in the target list")
    {
        Command command("move", { "aaaa" }, { { "to", "tempList2Name" } }, "move aaaa --to tempList2Name");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        DescriptionRepository descriptionRepository(configService.getDescriptionsDirPath());
        DescriptionService descriptionService(init.getCacheDirPath(), descriptionRepository);

        ListItemService listItemService(
            ioService, configService, listItemRepository, descriptionRepository, priorityService, statusService);
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

    SECTION("move of several items only moves the existing descriptions")
    {
        Command command("move", { "aaaa", "bbbb" }, { { "to", "tempList2Name" } }, "move aaaa bbbb --to tempList2Name");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        DescriptionRepository descriptionRepository(configService.getDescriptionsDirPath());
        DescriptionService descriptionService(init.getCacheDirPath(), descriptionRepository);

        ListItemService listItemService(
            ioService, configService, listItemRepository, descriptionRepository, priorityService, statusService);
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

    SECTION("move overwriting an item removes the target description when the source has none")
    {
        std::map<std::string, std::string> forceOpt = { { "force", "" }, { "to", "tempList2Name" } };
        Command command("move", { "aaaa" }, forceOpt, "move aaaa --to tempList2Name --force");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        DescriptionRepository descriptionRepository(configService.getDescriptionsDirPath());
        DescriptionService descriptionService(init.getCacheDirPath(), descriptionRepository);

        ListItemService listItemService(
            ioService, configService, listItemRepository, descriptionRepository, priorityService, statusService);
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

    SECTION("move overwriting an item replaces the target description with the source one")
    {
        std::map<std::string, std::string> forceOpt = { { "force", "" }, { "to", "tempList2Name" } };
        Command command("move", { "aaaa" }, forceOpt, "move aaaa --to tempList2Name --force");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        DescriptionRepository descriptionRepository(configService.getDescriptionsDirPath());
        DescriptionService descriptionService(init.getCacheDirPath(), descriptionRepository);

        ListItemService listItemService(
            ioService, configService, listItemRepository, descriptionRepository, priorityService, statusService);
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

        ListItemService listItemService(
            ioService, configService, listItemRepository, descriptionRepository, priorityService, statusService);
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

        ListItemService listItemService(
            ioService, configService, listItemRepository, descriptionRepository, priorityService, statusService);
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
        auto newItem = std::ranges::find_if(
            items, [](const ListItemEntity& item) { return *item.getId() != "aaaa" && *item.getId() != "bbbb"; });
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

        ListItemService listItemService(
            ioService, configService, listItemRepository, descriptionRepository, priorityService, statusService);
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

    // ---- parent / child: children and their descriptions follow ---------------------------

    // tempListName (active) holds aaaa + bbbb; bbbb is made a child of aaaa, with a description. The target
    // list gets its own unrelated "bbbb" (with a description), so the moved child needs a new id.
    auto readDescription = [](const std::filesystem::path& path) {
        std::ifstream file(path);
        std::stringstream content;
        content << file.rdbuf();
        return content.str();
    };

    for (const std::string action : { "move", "copy" }) {
        SECTION(action + " a parent: a child with a colliding id gets a new id, its description follows it")
        {
            Command command(action, { "aaaa" }, { { "to", "tempList2Name" } }, action + " aaaa --to tempList2Name");
            ConfigService configService(ioService, init, configRepository, cacheRepository, command);
            ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
            DescriptionRepository descriptionRepository(configService.getDescriptionsDirPath());
            ListItemService listItemService(
                ioService, configService, listItemRepository, descriptionRepository, priorityService, statusService);
            std::unique_ptr<DataSerializerInterface> listStoragePtr = std::make_unique<JsonSerializer>(ioService);
            ListRepository listRepository(configService, listStoragePtr.get());
            ListService listService(ioService, configService, listRepository, bus);
            ListName listName = listService.createUsedListName();
            ListName targetList = listService.createListName("tempList2Name");
            DescriptionService descriptionService(init.getCacheDirPath(), descriptionRepository);

            listItemService.attach("bbbb", "aaaa", listName);
            ListItemEntity unrelated = listItemRepository.find("cccc", targetList);
            unrelated.setId("bbbb");
            listItemRepository.create(unrelated, targetList);

            auto writeDescription = [&](const std::string& id, ListName& ln, const std::string& text) {
                std::filesystem::path path = descriptionRepository.getFilePath(id, ln);
                std::filesystem::create_directories(path.parent_path());
                std::ofstream(path) << text;
            };
            writeDescription("bbbb", listName, "child notes");
            writeDescription("bbbb", targetList, "unrelated notes");

            if (action == "move") {
                MoveItemAction move(ioService, listService, listItemService, descriptionService);
                REQUIRE_NOTHROW(move.execute(command, listName));
            } else {
                CopyItemAction copy(ioService, listService, listItemService, descriptionService);
                REQUIRE_NOTHROW(copy.execute(command, listName));
            }

            // the child got a new id in the target list, still attached to aaaa
            std::vector<ListItemEntity> children = listItemService.getChildren("aaaa", targetList);
            REQUIRE(children.size() == 1);
            std::string newChildId = *children[0].getId();
            REQUIRE(newChildId != "bbbb");
            REQUIRE(*children[0].getValue() == "test 2");

            // its description followed it; the unrelated bbbb kept its own
            REQUIRE(readDescription(descriptionRepository.getFilePath(newChildId, targetList)) == "child notes");
            REQUIRE(readDescription(descriptionRepository.getFilePath("bbbb", targetList)) == "unrelated notes");
            REQUIRE(listItemService.find("bbbb", targetList).hasParent() == false);

            if (action == "move") {
                REQUIRE(listItemService.get(listName).empty());
                REQUIRE_FALSE(descriptionRepository.exists("bbbb", listName));
            } else {
                REQUIRE(listItemService.get(listName).size() == 2);
                REQUIRE(readDescription(descriptionRepository.getFilePath("bbbb", listName)) == "child notes");
            }

            installation.wipe();
            installation.make();
        }
    }
}
