#include "../../Actions/ListAction/AddListAction.h"
#include "../../Actions/ListAction/CopyListAction.h"
#include "../../Actions/ListAction/RemoveListAction.h"
#include "../../Actions/ListAction/RenameListAction.h"
#include "../../Actions/ListAction/ShowListAction.h"
#include "../../Command/CommandOption.h"
#include "../../Command/CommandRegistry.h"
#include "../../Command/CommandService.h"
#include "../../Config/ConfigService.h"
#include "../../Describe/DescriptionService.h"
#include "../../Events/EventBus.h"
#include "../../FileDataStorageRepositories/ConfigRepository.h"
#include "../../FileDataStorageRepositories/ListItemRepository.h"
#include "../../FileDataStorageRepositories/ListRepository.h"
#include "../../FileStorage/FileStorageService.h"
#include "../../FileDataStorageRepositories/DescriptionRepository.h"
#include "../../List/ListItemService.h"
#include "../../List/ListItems/PriorityService.h"
#include "../../List/ListItems/StatusService.h"
#include "../../List/ListService.h"
#include "../../Serializers/ConfSerializer.h"
#include "../../Serializers/JsonSerializer.h"
#include "../../Themes/ThemeService.h"
#include "../../UseCase/AddListUseCase.h"
#include "../../UseCase/CopyListUseCase.h"
#include "../../UseCase/MoveListUseCase.h"
#include "../../UseCase/RemoveListUseCase.h"
#include "../../UseCase/RenameListUseCase.h"
#include "../../UseCase/ShowListUseCase.h"
#include "../../UseCase/SwitchListUseCase.h"
#include "../Mock/MockAppInitialization.h"
#include "../Mock/MockAppInstallation.h"
#include <catch2/catch_test_macros.hpp>
#include <fstream>
#include <filesystem>

TEST_CASE("ListAction action", "[ListAction]")
{
    IOService ioService("cli");
    ConfSerializer confService(ioService);
    JsonSerializer jsonService(ioService);
    std::unique_ptr<DataSerializerInterface> storagePtr = std::make_unique<JsonSerializer>(ioService);
    std::unique_ptr<DataSerializerInterface> configStoragePtr = std::make_unique<ConfSerializer>(ioService);

    MockAppInitialization init(ioService, "_todoos_ListActionsTest");
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

    // ---- ShowListAction — show/empty subcommand ----------------------------------------

    SECTION("show — no subcommand shows list without throwing")
    {
        Command command("list", {}, {}, "list");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        DescriptionRepository descriptionRepository(configService.getDescriptionsDirPath());

        ListItemService listItemService(ioService, configService, listItemRepository, descriptionRepository, priorityService, statusService);
        std::unique_ptr<DataSerializerInterface> listStoragePtr = std::make_unique<JsonSerializer>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ThemeService themeService(ioService, configService, listService, listItemService);

        ShowListAction action(ioService, listService, configService, themeService);
        REQUIRE_NOTHROW(action.execute(command));
    }

    // ---- AddListAction — add subcommand -----------------------------------------------

    SECTION("add — with name creates new list")
    {
        Command command("list", { "add", "newTestList" }, {}, "list add newTestList");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        DescriptionRepository descriptionRepository(configService.getDescriptionsDirPath());

        ListItemService listItemService(ioService, configService, listItemRepository, descriptionRepository, priorityService, statusService);
        std::unique_ptr<DataSerializerInterface> listStoragePtr = std::make_unique<JsonSerializer>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        FileStorageService fileStorageService(ioService, configService);

        std::size_t countBefore = listService.get().size();

        Command subCommand("add", { "newTestList" }, {}, "add newTestList");
        AddListAction action(ioService, listService, fileStorageService);
        REQUIRE_NOTHROW(action.execute(subCommand));

        std::size_t countAfter = listService.get().size();
        REQUIRE(countAfter == countBefore + 1);

        installation.wipe();
        installation.make();
    }

    SECTION("add — with no name does not throw")
    {
        Command command("list", { "add" }, {}, "list add");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        DescriptionRepository descriptionRepository(configService.getDescriptionsDirPath());

        ListItemService listItemService(ioService, configService, listItemRepository, descriptionRepository, priorityService, statusService);
        std::unique_ptr<DataSerializerInterface> listStoragePtr = std::make_unique<JsonSerializer>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        FileStorageService fileStorageService(ioService, configService);

        Command subCommand("add", {}, {}, "add");
        AddListAction action(ioService, listService, fileStorageService);
        REQUIRE_NOTHROW(action.execute(subCommand));
    }

    // ---- RemoveListAction — remove subcommand --------------------------------------------

    SECTION("remove — existing list removes it")
    {
        Command command("list", { "remove", "tempList2Name" }, {}, "list remove tempList2Name");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        DescriptionRepository descriptionRepository(configService.getDescriptionsDirPath());

        ListItemService listItemService(ioService, configService, listItemRepository, descriptionRepository, priorityService, statusService);
        std::unique_ptr<DataSerializerInterface> listStoragePtr = std::make_unique<JsonSerializer>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        FileStorageService fileStorageService(ioService, configService);

        std::size_t countBefore = listService.get().size();

        Command subCommand("remove", { "tempList2Name" }, {}, "remove tempList2Name");
        RemoveListAction action(ioService, listService, fileStorageService, configService);
        REQUIRE_NOTHROW(action.execute(subCommand));

        std::size_t countAfter = listService.get().size();
        REQUIRE(countAfter == countBefore - 1);

        installation.wipe();
        installation.make();
    }

    SECTION("remove — with no name does not throw")
    {
        Command command("list", { "remove" }, {}, "list remove");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        DescriptionRepository descriptionRepository(configService.getDescriptionsDirPath());

        ListItemService listItemService(ioService, configService, listItemRepository, descriptionRepository, priorityService, statusService);
        std::unique_ptr<DataSerializerInterface> listStoragePtr = std::make_unique<JsonSerializer>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        FileStorageService fileStorageService(ioService, configService);

        Command subCommand("remove", {}, {}, "remove");
        RemoveListAction action(ioService, listService, fileStorageService, configService);
        REQUIRE_NOTHROW(action.execute(subCommand));
    }

    SECTION("remove — without force keeps a backup of the list files")
    {
        Command command("list", { "remove", "tempList2Name" }, {}, "list remove tempList2Name");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        std::unique_ptr<DataSerializerInterface> listStoragePtr = std::make_unique<JsonSerializer>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        FileStorageService fileStorageService(ioService, configService);

        Command subCommand("remove", { "tempList2Name" }, {}, "remove tempList2Name");
        RemoveListAction action(ioService, listService, fileStorageService, configService);
        REQUIRE_NOTHROW(action.execute(subCommand));

        REQUIRE_FALSE(std::filesystem::exists(configService.getListFilePath("tempList2Name")));
        REQUIRE_FALSE(std::filesystem::exists(configService.getListArchiveFilePath("tempList2Name")));
        REQUIRE_FALSE(std::filesystem::exists(configService.getListDeleteFilePath("tempList2Name")));
        REQUIRE(std::filesystem::exists(configService.getListFilePath("._backup_del_tempList2Name")));
        REQUIRE(std::filesystem::exists(configService.getListArchiveFilePath("._backup_del_tempList2Name")));
        REQUIRE(std::filesystem::exists(configService.getListDeleteFilePath("._backup_del_tempList2Name")));

        installation.wipe();
        installation.make();
    }

    SECTION("remove — with force deletes the list files without backup")
    {
        std::map<std::string, std::string> forceOpt = { { "force", "" } };
        Command command("list", { "remove", "tempList2Name" }, forceOpt, "list remove --force tempList2Name");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        std::unique_ptr<DataSerializerInterface> listStoragePtr = std::make_unique<JsonSerializer>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        FileStorageService fileStorageService(ioService, configService);

        Command subCommand("remove", { "tempList2Name" }, forceOpt, "remove --force tempList2Name");
        RemoveListAction action(ioService, listService, fileStorageService, configService);
        REQUIRE_NOTHROW(action.execute(subCommand));

        REQUIRE_FALSE(std::filesystem::exists(configService.getListFilePath("tempList2Name")));
        REQUIRE_FALSE(std::filesystem::exists(configService.getListArchiveFilePath("tempList2Name")));
        REQUIRE_FALSE(std::filesystem::exists(configService.getListDeleteFilePath("tempList2Name")));
        REQUIRE_FALSE(std::filesystem::exists(configService.getListFilePath("._backup_del_tempList2Name")));
        REQUIRE_FALSE(std::filesystem::exists(configService.getListArchiveFilePath("._backup_del_tempList2Name")));
        REQUIRE_FALSE(std::filesystem::exists(configService.getListDeleteFilePath("._backup_del_tempList2Name")));

        installation.wipe();
        installation.make();
    }

    SECTION("remove — with force removes the list from the list of lists")
    {
        std::map<std::string, std::string> forceOpt = { { "force", "" } };
        Command command("list", { "remove", "tempList2Name" }, forceOpt, "list remove --force tempList2Name");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        std::unique_ptr<DataSerializerInterface> listStoragePtr = std::make_unique<JsonSerializer>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        FileStorageService fileStorageService(ioService, configService);

        std::size_t countBefore = listService.get().size();

        Command subCommand("remove", { "tempList2Name" }, forceOpt, "remove --force tempList2Name");
        RemoveListAction action(ioService, listService, fileStorageService, configService);
        REQUIRE_NOTHROW(action.execute(subCommand));

        REQUIRE(listService.get().size() == countBefore - 1);
        REQUIRE_THROWS(listService.find("tempList2Name"));

        installation.wipe();
        installation.make();
    }

    SECTION("remove — with force on the used list switches back to the default list")
    {
        std::map<std::string, std::string> forceOpt = { { "force", "" } };
        Command command("list", { "remove", "tempListName" }, forceOpt, "list remove --force tempListName");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        std::unique_ptr<DataSerializerInterface> listStoragePtr = std::make_unique<JsonSerializer>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        FileStorageService fileStorageService(ioService, configService);

        configService.edit("defaultList", "tempList2Name");
        REQUIRE(configService.getUsedListNameStr() == "tempListName");

        Command subCommand("remove", { "tempListName" }, forceOpt, "remove --force tempListName");
        RemoveListAction action(ioService, listService, fileStorageService, configService);
        REQUIRE_NOTHROW(action.execute(subCommand));

        REQUIRE(configService.getUsedListNameStr() == "tempList2Name");
        REQUIRE_FALSE(std::filesystem::exists(configService.getListFilePath("tempListName")));

        installation.wipe();
        installation.make();
    }

    SECTION("remove — with force on another list keeps the used list")
    {
        std::map<std::string, std::string> forceOpt = { { "force", "" } };
        Command command("list", { "remove", "tempList2Name" }, forceOpt, "list remove --force tempList2Name");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        std::unique_ptr<DataSerializerInterface> listStoragePtr = std::make_unique<JsonSerializer>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        FileStorageService fileStorageService(ioService, configService);

        Command subCommand("remove", { "tempList2Name" }, forceOpt, "remove --force tempList2Name");
        RemoveListAction action(ioService, listService, fileStorageService, configService);
        REQUIRE_NOTHROW(action.execute(subCommand));

        REQUIRE(configService.getUsedListNameStr() == "tempListName");
        REQUIRE(std::filesystem::exists(configService.getListFilePath("tempListName")));

        installation.wipe();
        installation.make();
    }

    SECTION("remove — with force on an unknown list changes nothing")
    {
        std::map<std::string, std::string> forceOpt = { { "force", "" } };
        Command command("list", { "remove", "unknownList" }, forceOpt, "list remove --force unknownList");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        std::unique_ptr<DataSerializerInterface> listStoragePtr = std::make_unique<JsonSerializer>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        FileStorageService fileStorageService(ioService, configService);

        std::size_t countBefore = listService.get().size();

        Command subCommand("remove", { "unknownList" }, forceOpt, "remove --force unknownList");
        RemoveListAction action(ioService, listService, fileStorageService, configService);
        REQUIRE_NOTHROW(action.execute(subCommand));

        REQUIRE(listService.get().size() == countBefore);
        REQUIRE(configService.getUsedListNameStr() == "tempListName");
        REQUIRE(std::filesystem::exists(configService.getListFilePath("tempListName")));
        REQUIRE(std::filesystem::exists(configService.getListFilePath("tempList2Name")));
    }

    // ---- RenameListAction — move subcommand -------------------------------------------

    SECTION("rename — move renames a list")
    {
        Command command(
            "list", { "move", "tempListName", "renamedList" }, {}, "list move tempListName renamedList");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        DescriptionRepository descriptionRepository(configService.getDescriptionsDirPath());
        DescriptionService descriptionService(init.getCacheDirPath(), descriptionRepository);

        ListItemService listItemService(ioService, configService, listItemRepository, descriptionRepository, priorityService, statusService);
        std::unique_ptr<DataSerializerInterface> listStoragePtr = std::make_unique<JsonSerializer>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        FileStorageService fileStorageService(ioService, configService);

        Command subCommand("move", { "tempListName", "renamedList" }, {}, "move tempListName renamedList");
        RenameListAction action(ioService, listService, fileStorageService, configService, descriptionService);
        REQUIRE_NOTHROW(action.execute(subCommand));

        bool found = false;
        for (auto& list : listService.get()) {
            if (*list.getName() == "renamedList") {
                found = true;
                break;
            }
        }
        REQUIRE(found);

        installation.wipe();
        installation.make();
    }

    SECTION("rename — move with too few arguments does not throw")
    {
        Command command("list", { "move", "tempListName" }, {}, "list move tempListName");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        DescriptionRepository descriptionRepository(configService.getDescriptionsDirPath());
        DescriptionService descriptionService(init.getCacheDirPath(), descriptionRepository);

        ListItemService listItemService(ioService, configService, listItemRepository, descriptionRepository, priorityService, statusService);
        std::unique_ptr<DataSerializerInterface> listStoragePtr = std::make_unique<JsonSerializer>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        FileStorageService fileStorageService(ioService, configService);

        Command subCommand("move", { "tempListName" }, {}, "move tempListName");
        RenameListAction action(ioService, listService, fileStorageService, configService, descriptionService);
        REQUIRE_NOTHROW(action.execute(subCommand));
    }

    SECTION("rename — move moves the list descriptions to the renamed list")
    {
        Command command(
            "list", { "move", "tempListName", "renamedList" }, {}, "list move tempListName renamedList");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        DescriptionRepository descriptionRepository(configService.getDescriptionsDirPath());
        DescriptionService descriptionService(init.getCacheDirPath(), descriptionRepository);

        ListItemService listItemService(ioService, configService, listItemRepository, descriptionRepository, priorityService, statusService);
        std::unique_ptr<DataSerializerInterface> listStoragePtr = std::make_unique<JsonSerializer>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        FileStorageService fileStorageService(ioService, configService);
        ListName sourceList = listService.createListName("tempListName");

        std::filesystem::path descriptionPath = descriptionRepository.getFilePath("bbbb", sourceList);
        std::filesystem::create_directories(descriptionPath.parent_path());
        std::ofstream(descriptionPath) << "Description of bbbb.\n";

        Command subCommand("move", { "tempListName", "renamedList" }, {}, "move tempListName renamedList");
        RenameListAction action(ioService, listService, fileStorageService, configService, descriptionService);
        REQUIRE_NOTHROW(action.execute(subCommand));

        ListName renamedList = listService.createListName("renamedList");
        REQUIRE_FALSE(std::filesystem::exists(descriptionRepository.getListDirPath(sourceList)));
        REQUIRE(descriptionRepository.exists("bbbb", renamedList));
        REQUIRE_FALSE(descriptionRepository.exists("aaaa", renamedList));

        installation.wipe();
        installation.make();
    }

    // ---- CopyListAction — copy subcommand -------------------------------------------------

    SECTION("copy — copies the list descriptions to the new list")
    {
        Command command("list", { "copy", "tempListName", "copiedList" }, {}, "list copy tempListName copiedList");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        DescriptionRepository descriptionRepository(configService.getDescriptionsDirPath());
        DescriptionService descriptionService(init.getCacheDirPath(), descriptionRepository);

        ListItemService listItemService(ioService, configService, listItemRepository, descriptionRepository, priorityService, statusService);
        std::unique_ptr<DataSerializerInterface> listStoragePtr = std::make_unique<JsonSerializer>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        FileStorageService fileStorageService(ioService, configService);
        ListName sourceList = listService.createListName("tempListName");

        std::filesystem::path descriptionPath = descriptionRepository.getFilePath("bbbb", sourceList);
        std::filesystem::create_directories(descriptionPath.parent_path());
        std::ofstream(descriptionPath) << "Description of bbbb.\n";

        Command subCommand("copy", { "tempListName", "copiedList" }, {}, "copy tempListName copiedList");
        CopyListAction action(ioService, listService, listItemService, fileStorageService, descriptionService);
        REQUIRE_NOTHROW(action.execute(subCommand));

        ListName copiedList = listService.createListName("copiedList");
        REQUIRE(listItemService.get(copiedList).size() == 2);
        REQUIRE(descriptionRepository.exists("bbbb", sourceList));
        REQUIRE(descriptionRepository.exists("bbbb", copiedList));
        REQUIRE_FALSE(descriptionRepository.exists("aaaa", copiedList));

        installation.wipe();
        installation.make();
    }

    SECTION("copy — keeps the parent / child links and the children's descriptions")
    {
        Command command("list", { "copy", "tempListName", "copiedList" }, {}, "list copy tempListName copiedList");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        DescriptionRepository descriptionRepository(configService.getDescriptionsDirPath());
        DescriptionService descriptionService(init.getCacheDirPath(), descriptionRepository);

        ListItemService listItemService(ioService, configService, listItemRepository, descriptionRepository, priorityService, statusService);
        std::unique_ptr<DataSerializerInterface> listStoragePtr = std::make_unique<JsonSerializer>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        FileStorageService fileStorageService(ioService, configService);
        ListName sourceList = listService.createListName("tempListName");

        listItemService.attach("bbbb", "aaaa", sourceList);
        std::filesystem::path descriptionPath = descriptionRepository.getFilePath("bbbb", sourceList);
        std::filesystem::create_directories(descriptionPath.parent_path());
        std::ofstream(descriptionPath) << "Description of bbbb.\n";

        Command subCommand("copy", { "tempListName", "copiedList" }, {}, "copy tempListName copiedList");
        CopyListAction action(ioService, listService, listItemService, fileStorageService, descriptionService);
        REQUIRE_NOTHROW(action.execute(subCommand));

        ListName copiedList = listService.createListName("copiedList");
        REQUIRE(listItemService.get(copiedList).size() == 2);
        REQUIRE(*listItemService.find("bbbb", copiedList).getParentId() == "aaaa");
        REQUIRE(descriptionRepository.exists("bbbb", copiedList));

        installation.wipe();
        installation.make();
    }

    // ---- ListUseCase — current subcommand ---------------------------------------------

    SECTION("list current — does not throw")
    {
        Command command("list", { "current" }, {}, "list current");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        DescriptionRepository descriptionRepository(configService.getDescriptionsDirPath());

        ListItemService listItemService(ioService, configService, listItemRepository, descriptionRepository, priorityService, statusService);
        std::unique_ptr<DataSerializerInterface> listStoragePtr = std::make_unique<JsonSerializer>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        FileStorageService fileStorageService(ioService, configService);
        ThemeService themeService(ioService, configService, listService, listItemService);

        ShowListUseCase useCase(ioService, listService, configService, themeService);
        REQUIRE_NOTHROW(useCase.execute(command));
    }

    // ---- ListUseCase — invalid subcommand ---------------------------------------------

    SECTION("list — invalid subcommand shows error without throwing")
    {
        Command command("list", { "badsubcmd" }, {}, "list badsubcmd");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        DescriptionRepository descriptionRepository(configService.getDescriptionsDirPath());

        ListItemService listItemService(ioService, configService, listItemRepository, descriptionRepository, priorityService, statusService);
        std::unique_ptr<DataSerializerInterface> listStoragePtr = std::make_unique<JsonSerializer>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        FileStorageService fileStorageService(ioService, configService);
        ThemeService themeService(ioService, configService, listService, listItemService);

        SwitchListUseCase useCase(
            ioService, commandService, listService, listItemService, fileStorageService, configService, themeService);
        REQUIRE_NOTHROW(useCase.execute(command));
    }

    // ---- SwitchListUseCase ---------------------------------------------------------------

    SECTION("use — with valid list name switches active list")
    {
        Command command("use", { "tempList2Name" }, {}, "use tempList2Name");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        DescriptionRepository descriptionRepository(configService.getDescriptionsDirPath());

        ListItemService listItemService(ioService, configService, listItemRepository, descriptionRepository, priorityService, statusService);
        std::unique_ptr<DataSerializerInterface> listStoragePtr = std::make_unique<JsonSerializer>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        FileStorageService fileStorageService(ioService, configService);
        ThemeService themeService(ioService, configService, listService, listItemService);

        SwitchListUseCase useCase(
            ioService, commandService, listService, listItemService, fileStorageService, configService, themeService);
        REQUIRE_NOTHROW(useCase.execute(command));
    }

    SECTION("use — with empty arguments switches to default list")
    {
        Command command("use", {}, {}, "use");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        DescriptionRepository descriptionRepository(configService.getDescriptionsDirPath());

        ListItemService listItemService(ioService, configService, listItemRepository, descriptionRepository, priorityService, statusService);
        std::unique_ptr<DataSerializerInterface> listStoragePtr = std::make_unique<JsonSerializer>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        FileStorageService fileStorageService(ioService, configService);
        ThemeService themeService(ioService, configService, listService, listItemService);

        SwitchListUseCase useCase(
            ioService, commandService, listService, listItemService, fileStorageService, configService, themeService);
        REQUIRE_NOTHROW(useCase.execute(command));
    }

    SECTION("use — with non-existent list does not throw")
    {
        Command command("use", { "noSuchList" }, {}, "use noSuchList");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        DescriptionRepository descriptionRepository(configService.getDescriptionsDirPath());

        ListItemService listItemService(ioService, configService, listItemRepository, descriptionRepository, priorityService, statusService);
        std::unique_ptr<DataSerializerInterface> listStoragePtr = std::make_unique<JsonSerializer>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        FileStorageService fileStorageService(ioService, configService);
        ThemeService themeService(ioService, configService, listService, listItemService);

        SwitchListUseCase useCase(
            ioService, commandService, listService, listItemService, fileStorageService, configService, themeService);
        REQUIRE_NOTHROW(useCase.execute(command));
    }
}
